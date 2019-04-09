<template>
    <canvas id="result-canvas" :style="img_size_style"></canvas>
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
            force_point(a, b, pow, delta) {
                const max_force = 0.001;

                a.crazy_seed = (a.crazy_seed + delta * a.crazy_speed) % (Math.PI * 2);

                let ba = {x: b.x - a._x, y: b.y - a._y};
                let ba_len = (ba.x ** 2 + ba.y ** 2) ** 0.5;

                let ivel_len = (ba_len * 2 * max_force) ** 0.5;
                let ivel = {x: ba.x / ba_len * ivel_len, y: ba.y / ba_len * ivel_len};

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
            draw() {
                const now = performance.now();
                const delta = now - this.last_time;
                this.last_time = now;

                this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
                this.ctx.fillStyle = GenProcess.config.background_color;
                this.ctx.fillRect(0, 0, this.canvas.width, this.canvas.height);

                if (GenProcess.edges !== null)
                    this.draw_edges(delta);

                requestAnimationFrame(this.draw);
            },
            draw_edges(delta) {
                const pow = 0.7 ** (delta / 350);

                for (let i = 0; i < GenProcess.config.segments_cnt; ++i) {
                    let gen_segment = GenProcess.edges.segments[i];
                    let na = { x: gen_segment[0], y: gen_segment[1] };
                    let nb = { x: gen_segment[2], y: gen_segment[3] };

                    let segment = this.edges.segments[i];
                    this.force_point(segment.a, na, pow, delta);
                    this.force_point(segment.b, nb, pow, delta);

                    this.ctx.strokeStyle = GenProcess.config.edges_color;
                    this.ctx.beginPath();
                    this.ctx.moveTo(this.edges.segments[i].a.x, this.edges.segments[i].a.y);
                    this.ctx.lineTo(this.edges.segments[i].b.x, this.edges.segments[i].b.y);
                    this.ctx.closePath();
                    this.ctx.stroke();
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
