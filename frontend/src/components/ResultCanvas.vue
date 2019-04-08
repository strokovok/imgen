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
                    x: this.canvas.width / 2,
                    y: this.canvas.height / 2,
                }
            },
            force_point(a, b, pow) {
                return {
                    x: b.x - (b.x - a.x) * pow,
                    y: b.y - (b.y - a.y) * pow,
                };
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
                const pow = 0.6 ** (delta / 200);

                for (let i = 0; i < GenProcess.config.segments_cnt; ++i) {
                    let segment = GenProcess.edges.segments[i];
                    let na = { x: segment[0], y: segment[1] };
                    let nb = { x: segment[2], y: segment[3] };
                    this.edges.segments[i].a = this.force_point(this.edges.segments[i].a, na, pow);
                    this.edges.segments[i].b = this.force_point(this.edges.segments[i].b, nb, pow);

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
