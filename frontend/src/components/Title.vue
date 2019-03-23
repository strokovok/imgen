<template>
    <div class="title"
         :class="{'small': is_small}"
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
            margin-bottom: 300px;
            transform: rotate(720deg);
        }
    }
</style>

<script>
    export default {
        data() {
            return {
                is_small: false,
                title: "IMGEN",
            };
        },
        methods: {
            change_size() {
                this.is_small = !this.is_small;
            }
        }
    }
</script>