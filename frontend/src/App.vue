<template>
    <div class="app">
        <Title></Title>
        <UserImage v-if="show_user_image"></UserImage>
        <UploadButton v-else></UploadButton>
    </div>
</template>

<style lang="scss" scoped>
    .app {
        display: flex;
        flex-direction: column;
        align-items: center;
    }
</style>

<script>
    import Title from './components/Title.vue';
    import UploadButton from './components/UploadButton.vue';
    import Session from './js/session.js';
    import SessionStates from './js/session_states.js';
    import UserImage from "./components/UserImage";

    export default {
        name: 'app',
        components: {
            UserImage,
            Title,
            UploadButton,
        },
        data() {
            return {

            }
        },
        computed: {
            show_user_image() {
                return Session.state !== SessionStates.START;
            }
        },
        created() {
            Session.start_socket();
        },
        methods: {

        },
    }
</script>
