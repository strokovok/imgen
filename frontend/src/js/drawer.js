import Vue from 'vue';
import UserImage from '@/js/user_image.js';
import GenProcess from '@/js/gen_process.js';

const drawer = new Vue({
    methods: {
        draw_background(ctx, canvas) {
            ctx.clearRect(0, 0, canvas.width, canvas.height);
            ctx.fillStyle = GenProcess.config.background_color;
            ctx.fillRect(0, 0, canvas.width, canvas.height);
        },
        draw_edge(ctx, edge) {
            ctx.strokeStyle = GenProcess.config.edges_color;
            ctx.lineWidth = 1;
            ctx.beginPath();
            ctx.moveTo(edge.a.x, edge.a.y);
            ctx.lineTo(edge.b.x, edge.b.y);
            ctx.closePath();
            ctx.stroke();
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
        draw_triangle(ctx, triangle) {
            const col = this.norm_color(triangle.color);
            const opacity = GenProcess.config.paint_opacity;
            ctx.fillStyle = `rgba(${col.r}, ${col.g}, ${col.b}, ${opacity})`;
            ctx.beginPath();
            ctx.moveTo(triangle.a.x, triangle.a.y);
            ctx.lineTo(triangle.b.x, triangle.b.y);
            ctx.lineTo(triangle.c.x, triangle.c.y);
            ctx.closePath();
            ctx.fill();
        },
        draw_circle(ctx, circle) {
            const col = this.norm_color(circle.color);
            const opacity = GenProcess.config.paint_opacity;
            ctx.fillStyle = `rgba(${col.r}, ${col.g}, ${col.b}, ${opacity})`;
            ctx.beginPath();
            ctx.arc(circle.p.x, circle.p.y, circle.radius, 0, Math.PI * 2);
            ctx.closePath();
            ctx.fill();
        },
        draw_paint_order(draw_t, draw_c) {
            if (GenProcess.paint !== null) {
                const tcnt = GenProcess.config.triangles_cnt;
                const ccnt = GenProcess.config.circles_cnt;
                let ti = 0, ci = 0;
                let done = true;
                done &= (ti === tcnt);
                done &= (ci === ccnt);
                while (!done) {
                    const tp = (tcnt > 0) ? ti / tcnt : 1e9;
                    const cp = (ccnt > 0) ? ci / ccnt : 1e9;
                    if (tp < cp)
                        draw_t(ti++);
                    else
                        draw_c(ci++);
                    done = true;
                    done &= (ti === tcnt);
                    done &= (ci === ccnt);
                }
            }
        },
        draw_all() {
            const resultCanvas = document.getElementById('result-canvas');

            const canvas = document.createElement('canvas');
            canvas.width = resultCanvas.width;
            canvas.height = resultCanvas.height;
            const ctx  =canvas.getContext('2d');

            this.draw_background(ctx, canvas);

            this.draw_paint_order(
                (ti) => {
                    const triangle = GenProcess.paint.triangles[ti];
                    this.draw_triangle(ctx, {
                        a: {x:triangle[0], y:triangle[1]},
                        b: {x:triangle[2], y:triangle[3]},
                        c: {x:triangle[4], y:triangle[5]},
                        color: {r:triangle[6], g:triangle[7], b:triangle[8]}
                    });
                },
                (ci) => {
                    const circle = GenProcess.paint.circles[ci];
                    this.draw_circle(ctx, {
                        p: {x:circle[0], y:circle[1]},
                        radius: circle[2],
                        color: {r:circle[3], g:circle[4], b:circle[5]}
                    });
                }
            );

            for (let i = 0; i < GenProcess.config.segments_cnt; ++i) {
                let edge = GenProcess.edges.segments[i];
                this.draw_edge(ctx, {
                    a: {x:edge[0], y:edge[1]},
                    b: {x:edge[2], y:edge[3]}
                });
            }

            return canvas;
        }
    }
});

export default drawer;
