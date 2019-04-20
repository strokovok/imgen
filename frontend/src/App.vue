<template>
    <div class="app">
        <app-title/>
        <transition-group class="main-elements" name="main-elements" tag="div">
            <user-image v-if="show_user_image" :key="'user-image'" class="main-elements-item"/>
            <div v-if="show_loading" :key="'loading'" class="processing main-elements-item">Обработка</div>
            <configuration v-if="show_configuration" :key="'configuration'" class="main-elements-item"/>
            <result-canvas v-if="show_result" :key="'result-canvas'" class="main-elements-item"/>
        </transition-group>
        <upload-button v-if="show_upload_button"></upload-button>
        <transition name="fade">
            <controls v-if="show_controls"></controls>
        </transition>
    </div>
</template>

<style lang="scss" scoped>
    .app {
        display: flex;
        flex-direction: column;
        align-items: center;
    }

    .main-elements {
        display: flex;
        align-items: center;
        justify-content: center;
        position: relative;
    }

    .main-elements-item {
        margin: 0 70px;
        transition: all .6s;
    }

    .processing {
        font-size: 20px;
        font-weight: 200;
    }

    .main-elements-enter, .main-elements-leave-to {
        opacity: 0;
    }

    .main-elements-leave-active {
        position: absolute;
    }

    .fade-enter-active, .fade-leave-active {
        transition: opacity 1s;
    }
    .fade-enter, .fade-leave-to {
        opacity: 0;
    }
</style>

<script>
    import AppTitle from './components/Title.vue';
    import UploadButton from './components/UploadButton.vue';
    import Session from './js/session.js';
    import SessionStates from './js/session_states.js';
    import UserImage from "./components/UserImage.vue";
    import Configuration from "./components/Configuration.vue";
    import ResultCanvas from "./components/ResultCanvas.vue";
    import Controls from './components/Controls.vue';

    export default {
        name: 'app',
        components: {
            UserImage,
            AppTitle,
            UploadButton,
            Configuration,
            ResultCanvas,
            Controls
        },
        computed: {
            show_upload_button() {
                return Session.state === SessionStates.START
            },
            show_user_image() {
                return Session.state !== SessionStates.START;
            },
            show_loading() {
                return Session.state === SessionStates.PROCESSING_FILE;
            },
            show_configuration() {
                return Session.state === SessionStates.READY_TO_RUN;
            },
            show_result() {
                return Session.state === SessionStates.RUNNING;
            },
            show_controls() {
                return Session.state === SessionStates.RUNNING;
            }
        },
        created() {
            Session.start_socket();
        },
    }
</script>
