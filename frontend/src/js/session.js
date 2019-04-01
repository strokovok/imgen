import Vue from 'vue';
import SessionStates from './session_states';
import BackOps from './back_ops.js';

const session = new Vue({
    data() {
        return {
            state: SessionStates.START,
            socket: null,
        };
    },
    methods: {
        start_socket() {
            let host = location.host;
            this.socket = new WebSocket("ws://" + host + "/ws");
            this.socket.onopen = () => this.on_socket_open();
            this.socket.onclose = (event) => this.on_socket_close(event);
            this.socket.onerror = (error) => this.on_socket_error(error);
            this.socket.onmessage = (message) => this.on_socket_message(message);
        },
        on_socket_open() {
            console.log("WebSocket opened");
        },
        on_socket_close(event) {
            console.log("WebSocket closed", event);
        },
        on_socket_error(error) {
            console.log("WebSocket error", error);
        },
        on_socket_message(message) {
            message = JSON.parse(message.data);
            //console.log("GOT MESSAGE", message);
            if (message["op"] === BackOps.BROKEN) {
                console.log("Broken:", message);
                alert("Что-то пошло не так, перезагрузите страницу.");
            }
            if (message["op"] === BackOps.READY_TO_RUN)
                setTimeout(() => {
                    this.state = SessionStates.READY_TO_RUN;
                }, 1000);
            this.$emit("on-socket-message", message);
        },
        send(message) {
            //console.log("SEND MESSAGE", message);
            this.socket.send(JSON.stringify(message));
        },
    }
});

export default session;
