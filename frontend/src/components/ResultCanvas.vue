<template>
    <canvas id="result-canvas" :style="img_size_style" @click="boom"></canvas>
</template>

<style lang="scss" scoped>
    #result-canvas {
        border-radius: 4px;
    }

</style>

<script>
    import UserImage from '@/js/user_image.js';
    import GenProcess from '@/js/gen_process.js';

    const MAX_WIDTH = 640;
    const MAX_HEIGHT = 640;

    export default {
        data() {
            return {
                canvas: null,
                ctx: null,
                edges: {
                    segments: []
                },
                paint: {
                    triangles: [],
                    circles: [],
                },
                last_time: null
            }
        },
        mounted() {
            this.canvas = document.getElementById("result-canvas");
            this.ctx = this.canvas.getContext("2d");
            let width = UserImage.image_data.width;
            let height = UserImage.image_data.height;
            let mul = GenProcess.CANVAS_SIZE / Math.max(width, height);
            this.canvas.width = width * mul;
            this.canvas.height = height * mul;
            requestAnimationFrame(this.draw);

            for (let i = 0; i < GenProcess.config.segments_cnt; ++i) {
                this.edges.segments.push({
                    a: this.make_middle_point(),
                    b: this.make_middle_point(),
                });
            }

            for (let i = 0; i < GenProcess.config.triangles_cnt; ++i) {
                this.paint.triangles.push({
                    a: this.make_middle_point(),
                    b: this.make_middle_point(),
                    c: this.make_middle_point(),
                    color: {
                        r: Math.random() * 255,
                        g: Math.random() * 255,
                        b: Math.random() * 255,
                    },
                });
            }

            for (let i = 0; i < GenProcess.config.circles_cnt; ++i) {
                this.paint.circles.push({
                    p: this.make_middle_point(),
                    radius: Math.random() * 10,
                    color: {
                        r: Math.random() * 255,
                        g: Math.random() * 255,
                        b: Math.random() * 255,
                    },
                });
            }

            this.last_time = performance.now();
        },
        methods: {
            make_middle_point() {
                return {
                    _x: this.canvas.width / 2,
                    _y: this.canvas.height / 2,
                    x_vel: 0,
                    y_vel: 0,
                    crazy_seed: Math.random() * Math.PI * 2,
                    crazy_radius: 1 + Math.random() * 4,
                    crazy_speed: (3 + Math.random() * 7) / 1000,
                };
            },
            evaluate_point(p) {
                p.x = p._x + Math.sin(p.crazy_seed) * p.crazy_radius;
                p.y = p._y + Math.cos(p.crazy_seed) * p.crazy_radius;
            },
            force_point(a, b, delta) {
                const max_force = 0.001;

                a.crazy_seed = (a.crazy_seed + delta * a.crazy_speed) % (Math.PI * 2);

                let ba = {x: b.x - a._x, y: b.y - a._y};
                let ba_len = (ba.x ** 2 + ba.y ** 2) ** 0.5;

                let vel = (a.x_vel ** 2 + a.y_vel ** 2) ** 0.5;
                if (ba_len < 1 && vel < 0.01) {
                    ba.x = ba.y = ba_len = a.x_vel = a.y_vel = 0;
                    a._x = b.x;
                    a._y = b.y;
                }

                let ivel_len = (ba_len * 2 * max_force) ** 0.5;
                let ivel = {x: 0, y: 0};
                if (ba_len > 0)
                    ivel = {x: ba.x / ba_len * ivel_len, y: ba.y / ba_len * ivel_len};

                let df = {x: ivel.x - a.x_vel, y: ivel.y - a.y_vel};
                let df_len = (df.x ** 2 + df.y ** 2) ** 0.5;

                let force = max_force * delta;

                let df_mul = (df_len > force) ? force / df_len : 1;
                a.x_vel += df.x * df_mul;
                a.y_vel += df.y * df_mul;

                a._x += a.x_vel * delta;
                a._y += a.y_vel * delta;

                this.evaluate_point(a);
            },
            force_color(a, b, pow) {
                a.r = b.r - (b.r - a.r) * pow;
                a.g = b.g - (b.g - a.g) * pow;
                a.b = b.b - (b.b - a.b) * pow;
            },
            draw() {
                const now = performance.now();
                let delta = now - this.last_time;
                if (delta > 1000)
                    delta = 30;
                this.last_time = now;

                this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
                this.ctx.fillStyle = GenProcess.config.background_color;
                this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

                const pow = 0.6 ** (delta / 700);

                if (GenProcess.paint !== null) {
                    const tcnt = GenProcess.config.triangles_cnt;
                    const ccnt = GenProcess.config.circles_cnt;
                    let ti = 0, ci = 0;
                    for (let done = false; !done;) {
                        const tp = (tcnt > 0) ? ti / tcnt : 1e9;
                        const cp = (ccnt > 0) ? ci / ccnt : 1e9;
                        if (tp < cp)
                            this.draw_triangle(ti++, delta, pow);
                        else
                            this.draw_circle(ci++, delta, pow);
                        done = true;
                        done &= (ti === tcnt);
                        done &= (ci === ccnt);
                    }
                }

                if (GenProcess.edges !== null)
                    this.draw_edges(delta);

                requestAnimationFrame(this.draw);
            },
            draw_edges(delta) {
                for (let i = 0; i < GenProcess.config.segments_cnt; ++i) {
                    let gen_segment = GenProcess.edges.segments[i];
                    let na = { x: gen_segment[0], y: gen_segment[1] };
                    let nb = { x: gen_segment[2], y: gen_segment[3] };

                    let segment = this.edges.segments[i];
                    this.force_point(segment.a, na, delta);
                    this.force_point(segment.b, nb, delta);

                    this.ctx.strokeStyle = GenProcess.config.edges_color;
                    this.ctx.lineWidth = 1;
                    this.ctx.beginPath();
                    this.ctx.moveTo(this.edges.segments[i].a.x, this.edges.segments[i].a.y);
                    this.ctx.lineTo(this.edges.segments[i].b.x, this.edges.segments[i].b.y);
                    this.ctx.closePath();
                    this.ctx.stroke();
                }
            },
            norm_channel(x) {
                return Math.max(Math.min(Math.floor(x), 255), 0);
            },
            norm_color(color) {
                return {
                    r: this.norm_channel(color.r),
                    g: this.norm_channel(color.g),
                    b: this.norm_channel(color.b)
                }
            },
            draw_triangle(i, delta, pow) {
                let gen_triangle = GenProcess.paint.triangles[i];
                let na = { x: gen_triangle[0], y: gen_triangle[1] };
                let nb = { x: gen_triangle[2], y: gen_triangle[3] };
                let nc = { x: gen_triangle[4], y: gen_triangle[5] };
                let ncolor = {
                    r: gen_triangle[6],
                    g: gen_triangle[7],
                    b: gen_triangle[8],
                };

                let triangle = this.paint.triangles[i];
                this.force_point(triangle.a, na, delta);
                this.force_point(triangle.b, nb, delta);
                this.force_point(triangle.c, nc, delta);
                this.force_color(triangle.color, ncolor, pow);

                const col = this.norm_color(triangle.color);
                const opacity = GenProcess.config.paint_opacity;
                this.ctx.fillStyle = `rgba(${col.r}, ${col.g}, ${col.b}, ${opacity})`;
                this.ctx.beginPath();
                this.ctx.moveTo(triangle.a.x, triangle.a.y);
                this.ctx.lineTo(triangle.b.x, triangle.b.y);
                this.ctx.lineTo(triangle.c.x, triangle.c.y);
                this.ctx.closePath();
                this.ctx.fill();
            },
            draw_circle(i, delta, pow) {
                let gen_circle = GenProcess.paint.circles[i];
                let np = { x: gen_circle[0], y: gen_circle[1] };
                let nradius = gen_circle[2];
                let ncolor = {
                    r: gen_circle[3],
                    g: gen_circle[4],
                    b: gen_circle[5],
                };

                let circle = this.paint.circles[i];
                this.force_point(circle.p, np, delta);
                circle.radius = nradius - (nradius - circle.radius) * pow;
                this.force_color(circle.color, ncolor, pow);

                const col = this.norm_color(circle.color);
                const opacity = GenProcess.config.paint_opacity;
                this.ctx.fillStyle = `rgba(${col.r}, ${col.g}, ${col.b}, ${opacity})`;
                this.ctx.beginPath();
                this.ctx.arc(circle.p.x, circle.p.y, circle.radius, 0, Math.PI * 2);
                this.ctx.closePath();
                this.ctx.fill();
            },
            boom_point(point, x, y) {
                point._x = x + Math.random() * 50 - 25;
                point._y = y + Math.random() * 50 - 25;
                const mx = 0.3;
                point.x_vel = Math.random() * mx - mx / 2;
                point.y_vel = Math.random() * mx - mx / 2;
            },
            boom(event) {
                if (!event.offsetY)
                    return;

                let x = event.offsetX / this.canvas.offsetWidth * this.canvas.width;
                let y = event.offsetY / this.canvas.offsetHeight * this.canvas.height;

                for (let seg of this.edges.segments) {
                    this.boom_point(seg.a, x, y);
                    this.boom_point(seg.b, x, y);
                }

                for (let tr of this.paint.triangles) {
                    this.boom_point(tr.a, x, y);
                    this.boom_point(tr.b, x, y);
                    this.boom_point(tr.c, x, y);
                }

                for (let c of this.paint.circles) {
                    this.boom_point(c.p, x, y);
                    c.radius = Math.random() * 10;
                }
            }
        },
        computed: {
            img_size_style() {
                let width = UserImage.image_data.width;
                let height = UserImage.image_data.height;
                let mul = (width > height) ? MAX_WIDTH / width : MAX_HEIGHT / height;
                width *= mul;
                height *= mul;
                return {
                    "width": `${width}px`,
                    "height": `${height}px`,
                }
            },
        },
    }
</script>
