<template>
    <div class="app">
        <app-title/>
        <transition-group class="main-elements" name="main-elements" tag="div">
            <user-image v-if="show_user_image" :key="'user-image'" class="main-elements-item"/>
            <div v-if="show_loading" :key="'loading'" class="processing main-elements-item">Обработка</div>
            <configuration v-if="show_configuration" :key="'configuration'" class="main-elements-item"/>
        </transition-group>
        <upload-button v-if="show_upload_button"></upload-button>
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
</style>

<script>
    import AppTitle from './components/Title.vue';
    import UploadButton from './components/UploadButton.vue';
    import Session from './js/session.js';
    import SessionStates from './js/session_states.js';
    import UserImage from "./components/UserImage.vue";
    import Configuration from "./components/Configuration.vue";

    export default {
        name: 'app',
        components: {
            UserImage,
            AppTitle,
            UploadButton,
            Configuration,
        },
        data() {
            return {

            }
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
        },
        created() {
            Session.start_socket();
        },
        methods: {

        },
    }
</script>
