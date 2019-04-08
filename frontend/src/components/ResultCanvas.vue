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

    const BACK_CANVAS_SIZE = 256;
    const MAX_CANVAS_SIZE = 1024;
    const CANVAS_MUL = MAX_CANVAS_SIZE / BACK_CANVAS_SIZE;

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
            let mul = MAX_CANVAS_SIZE / Math.max(width, height);
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
            force_point(a, b, delta) {
                const pow = 0.6 ** (delta / 200);
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

                for (let i = 0; i < GenProcess.config.segments_cnt; ++i) {
                    if (GenProcess.edges !== null) {
                        let na = {
                            x: GenProcess.edges.segments[i][0] * CANVAS_MUL,
                            y: GenProcess.edges.segments[i][1] * CANVAS_MUL,
                        };
                        let nb = {
                            x: GenProcess.edges.segments[i][2] * CANVAS_MUL,
                            y: GenProcess.edges.segments[i][3] * CANVAS_MUL,
                        };
                        this.edges.segments[i].a = this.force_point(this.edges.segments[i].a, na, delta);
                        this.edges.segments[i].b = this.force_point(this.edges.segments[i].b, nb, delta);
                    }

                    this.ctx.strokeStyle = GenProcess.config.edges_color;
                    this.ctx.beginPath();
                    this.ctx.moveTo(this.edges.segments[i].a.x, this.edges.segments[i].a.y);
                    this.ctx.lineTo(this.edges.segments[i].b.x, this.edges.segments[i].b.y);
                    this.ctx.closePath();
                    this.ctx.stroke();
                }

                requestAnimationFrame(this.draw);
            },
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
