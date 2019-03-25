<template>
    <div class="app">
        <Title></Title>
        <label class="ui-button upload-button">
            Выбрать изображение
            <input type="file" @change="on_file_choose">
        </label>
    </div>
</template>

<style lang="scss" scoped>
    .app {
        display: flex;
        flex-direction: column;
        align-items: center;
    }

    .upload-button {
        font-size: 30px;
        input {
            display: none;
        }
    }
</style>

<script>
    import Title from './components/Title.vue';

    export default {
        name: 'app',
        components: {
            Title,
        },
        data() {
            return {
                socket: null,
                pps: 0,
            }
        },
        mounted() {
            let host = location.host;
            this.socket = new WebSocket("ws://" + host + "/ws");
            this.socket.onopen = () => {
                console.log("Socket opened!");
            };
            this.socket.onmessage = (message) => {
                let data = JSON.parse(message.data);
                this.pps += data.length;
            };
            //setInterval(this.request_payload, 200);
            //setInterval(this.log_payload, 1000);
        },
        methods: {
            on_file_choose(a, b, c, d) {
                console.log("Kekus", a, b, c, d);
            },
            request_payload() {
                this.socket.send(JSON.stringify({"cnt": 100000}));
            },
            log_payload() {
                console.log(`PPS: ${this.pps}`);
                this.pps = 0;
            }
        }
    }
</script>
