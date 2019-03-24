<template>
    <div class="app">
        <Title></Title>
        <div class="ui-button upload-button">
            Выбрать изображение
            <input type="file" @change="on_file_choose">
        </div>
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
            }
        },
        mounted() {
            let host = location.host;
            this.socket = new WebSocket("ws://" + host + "/ws");
            this.socket.onopen = () => {
                this.socket.send("Привет!");
                console.log("Socket opened!");
            };
            this.socket.onmessage = (message) => {
                console.log(message);
            };
        },
        methods: {
            on_file_choose(a, b, c, d) {
                console.log("Kekus", a, b, c, d);
            }
        }
    }
</script>
