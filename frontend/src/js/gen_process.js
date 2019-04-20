import Vue from 'vue';
import Session from './session';
import SessionStates from './session_states';
import FrontOps from './front_ops';
import BackOps from './back_ops.js';

const gen_process = new Vue({
    data() {
        return {
            CANVAS_SIZE: 1024,
            edges: null,
            paint: null,
            config: null,
            time_start: 0,
            time: 0,
            iteration: 0,
            accuracy: 0,
        };
    },
    methods: {
        start(config) {
            this.config = config;
            Session.$on('on-socket-message', this.on_message);

            config["op"] = FrontOps.START;
            Session.send(config);

            this.time_start = performance.now();
            this.time = 0;
            requestAnimationFrame(this.updateTime);

            this.request_result(0);

            this.iteration = 0;
            this.accuracy = 0;

            Session.state = SessionStates.RUNNING;
        },
        updateTime() {
            this.time = Math.floor((performance.now() - this.time_start) / 100) / 10;
            requestAnimationFrame(this.updateTime);
        },
        request_result(delay) {
            setTimeout(() => {
                Session.send({
                    "op": FrontOps.GET_RESULT,
                });
            }, delay);
        },
        on_message(message) {
            if (message["op"] !== BackOps.RESULT)
                return;

            this.edges = message["edges"];
            const CANVAS_EDGES_MUL = this.CANVAS_SIZE / Session.consts.edges_canvas_size;
            for (let segment of this.edges.segments)
                for (let i = 0; i < 4; ++i)
                    segment[i] *= CANVAS_EDGES_MUL;

            this.paint = message["paint"];
            const CANVAS_PAINT_MUL = this.CANVAS_SIZE / Session.consts.paint_canvas_size;
            for (let triangle of this.paint.triangles)
                for (let i = 0; i < 6; ++i)
                    triangle[i] *= CANVAS_PAINT_MUL;
            for (let circle of this.paint.circles)
                for (let i = 0; i < 3; ++i)
                    circle[i] *= CANVAS_PAINT_MUL;

            this.iteration = message["iteration"];
            this.accuracy = message["accuracy"];

            this.request_result(1000);
        }
    }
});

export default gen_process;
