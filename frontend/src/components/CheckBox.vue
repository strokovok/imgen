<template>
    <label class="container">
        <slot></slot>
        <input type="checkbox" checked="checked">
        <span class="checkmark"></span>
    </label>
</template>

<style lang="scss" scoped>
    .container {
        display: block;
        position: relative;
        padding-left: 1.5em;
        cursor: pointer;
        -webkit-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
    }

    .container input {
        position: absolute;
        opacity: 0;
        cursor: pointer;
        height: 0;
        width: 0;
    }

    .checkmark {
        position: absolute;
        top: 0;
        left: 0;
        height: 1em;
        width: 1em;
        margin-top: 0.06em;
        border-radius: 4px;
        border: 1px white solid;
        transition: all .3s ease;
    }

    .container:hover input ~ .checkmark {
        background-color: transparentize(white, 0.9);
    }

    .container input:checked ~ .checkmark {
        //background-color: #2196F3;
    }

    .container input:active ~ .checkmark {
        background-color: transparentize(white, 0.8);
        transform: translate(0, 0.1em);
    }

    .checkmark:after {
        content: "";
        position: absolute;
        display: none;
    }

    .container input:checked ~ .checkmark:after {
        display: block;
    }

    .container .checkmark:after {
        left: 0.36em;
        top: 0.2em;
        width: 0.2em;
        height: 0.4em;
        border: solid white;
        border-width: 0 0.12em 0.12em 0;
        -webkit-transform: rotate(45deg);
        -ms-transform: rotate(45deg);
        transform: rotate(45deg);
    }
</style>

<script>

    export default {
        data() {
            return {

            };
        },
        computed: {

        },
        methods: {

        }
    }
</script>
