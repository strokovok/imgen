import Vue from 'vue';
import Session from './session';
import SessionStates from './session_states';
import FrontOps from './front_ops';
import BackOps from './back_ops.js';

const gen_process = new Vue({
    data() {
        return {
            edges: null,
            config: null,
        };
    },
    methods: {
        start(config) {
            this.config = config;
            Session.$on('on-socket-message', this.on_message);

            config["op"] = FrontOps.START;
            Session.send(config);

            this.request_result(0);

            Session.state = SessionStates.RUNNING;
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

            this.request_result(1000);
        }
    }
});

export default gen_process;
