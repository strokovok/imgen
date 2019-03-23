<template>
    <div class="app">
        <div class="title" :class="{'tight': tight_title}" @click="do_shit">IMGEN</div>
        <div class="upload">Загрузите жопу плз...</div>
        <!--<div class="title">Загрузите пизду на говно и выйдет хуйня</div>-->
    </div>
</template>

<style lang="scss" scoped>
    .app {
        display: flex;
        flex-direction: column;
        align-items: center;
    }

    .upload {
        font-size: 40px;
        opacity: 0.7;
        font-weight: 200;
    }

    .title {
        cursor: pointer;
        margin-top: 300px;
        font-weight: 700;
        font-size: 80px;
        opacity: 0.85;
        transition: all .9s ease;
        letter-spacing: 1em;
        padding-left: 1em;
        margin-bottom: 200px;
        &.tight {
            transform: rotate(720deg);
            letter-spacing: 0;
            padding-left: 0;
            margin-top: 20px;
            font-size: 50px;
            margin-bottom: 300px;
        }
        &.small {
            margin-top: 20px;
            font-size: 50px;
        }
    }
</style>

<script>
    //import HelloWorld from './components/HelloWorld.vue'

    export default {
        name: 'app',
        components: {
            //HelloWorld
        },
        data() {
            return {
                tight_title: true,
            }
        },
        methods: {
            do_shit() {
                this.tight_title = !this.tight_title;
            }
        }
    }
</script>
