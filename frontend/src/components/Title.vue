<template>
    <div class="title"
         :class="{'small': is_computed_small}"
         @click="change_size">
        {{title}}
    </div>
</template>

<style lang="scss" scoped>
    .title {
        cursor: pointer;
        opacity: 0.85;
        font-weight: 700;
        transition: all .9s ease;
        font-size: 80px;
        letter-spacing: 1em;
        padding-left: 1em;
        margin-top: 200px;
        margin-bottom: 200px;
        &.small {
            font-size: 50px;
            letter-spacing: 0;
            padding-left: 0;
            margin-top: 20px;
            margin-bottom: 100px;
            transform: rotate(720deg);
        }
    }
</style>

<script>
    import Session from '@/js/session.js';
    import SessionStates from '@/js/session_states.js';

    export default {
        data() {
            return {
                is_small: true,
                title: "IMGEN",
            };
        },
        computed: {
            resize_taboo() {
                return Session.state === SessionStates.START;
            },
            is_computed_small() {
                if (this.resize_taboo)
                    return false;
                return this.is_small;
            }
        },
        methods: {
            change_size() {
                if (this.resize_taboo)
                    return;
                this.is_small = !this.is_small;
            }
        }
    }
</script>
