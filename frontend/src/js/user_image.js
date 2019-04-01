import Vue from 'vue';
import Session from './session.js';
import SessionStates from './session_states.js';
import FrontOps from './front_ops.js';
import BackOps from './back_ops.js';

const PIECE_LEN = 1024;

const user_image = new Vue({
    data() {
        return {
            image_data: {
                extension: null,
                width: null,
                height: null,
                as_data_url: null,
                as_base64: null,
            },
            len_sent: 0,
            uploading_progress: 0,
        };
    },
    methods: {
        set_data(image_data) {
            for (let key in image_data)
                this.image_data[key] = image_data[key];
            Session.state = SessionStates.UPLOADING;
            this.start_uploading();
        },
        start_uploading() {
            this.len_sent = 0;
            this.uploading_progress = 0;
            Session.$on('on-socket-message', this.on_message);
            Session.send({
                "op": FrontOps.START_UPLOADING,
                "extension": this.image_data.extension,
                "length": this.image_data.as_base64.length,
            });
        },
        send_piece() {
            if (this.len_sent < this.image_data.as_base64.length) {
                const r = Math.min(this.len_sent + PIECE_LEN, this.image_data.as_base64.length);
                Session.send({
                    "op": FrontOps.UPLOAD_PIECE,
                    "piece": this.image_data.as_base64.substring(this.len_sent, r)
                });
                this.len_sent = r;
                this.uploading_progress = this.len_sent / this.image_data.as_base64.length;
            } else {
                Session.send({
                    "op": FrontOps.UPLOAD_DONE,
                });
                Session.$off('on-socket-message', this.on_message);
                Session.state = SessionStates.PROCESSING_FILE;
            }
        },
        on_message(message) {
            if (message["op"] === BackOps.GIVE_ME_PIECE)
                this.send_piece();
        }
    }
});

export default user_image;
