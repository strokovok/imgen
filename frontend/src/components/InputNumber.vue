<template>
    <input class="input-number" type="number" step="1" :min="min" :max="max" v-model="value" @change="on_change">
</template>

<style lang="scss" scoped>
    .input-number {
        outline: none;
        border: 1px transparentize(white, 0.1) solid;
        padding: 0.1em 0.2em;
        color: white;
        font-weight: 200;
        text-align: center;
        background: none;
        transition: all .3s ease;
        &:hover {
            background: transparentize(white, 0.9);
        }
        width: 3em;
    }

    input::-webkit-outer-spin-button,
    input::-webkit-inner-spin-button {
        -webkit-appearance: none;
        margin: 0;
    }

    input[type=number] {
        -moz-appearance:textfield;
    }
</style>

<script>
    export default {
        props: {
            min: {
                default: 0,
                type: Number,
            },
            max: {
                default: 999999999,
                type: Number,
            },
            number: Number,
        },
        model: {
            prop: 'number',
            event: 'change'
        },
        data() {
            return {
                value: "0"
            }
        },
        mounted() {
            this.value = this.number;
        },
        methods: {
            on_change() {
                let intVal = Number(this.value);
                if (!Number.isInteger(intVal))
                    intVal = 0;
                intVal = Math.max(Math.min(intVal, this.max), this.min);
                this.value = intVal;
                this.$emit('change', intVal);
            }
        }
    }
</script>
