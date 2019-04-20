<template>
    <div class="controls">
        <!--<div class="ui-button control-el">Заново</div>-->
        <!--<div class="ui-button control-el">Стоп</div>-->
        <!--<div class="ui-button control-el">Сохранить png</div>-->
        <!--<div class="ui-button control-el">Сохранить svg</div>-->
        <div class="control-el control-text">Время:<br>{{time}}с</div>
        <div class="control-el control-text">Итерация:<br>{{iteration_display}}</div>
        <div class="control-el control-text">Точность:<br>{{accuracy_display_formatted}}%</div>
    </div>
</template>

<style lang="scss" scoped>
    .controls {
        margin-top: 50px;
        margin-bottom: 50px;
        display: flex;
        align-items: center;
    }

    .control-el {
        margin: 0 20px;
    }

    .control-text {
        font-weight: 200;
        font-size: 20px;
        text-align: center;
    }

    .ui-button {
        font-size: 20px;
    }
</style>

<script>
    import GenProcess from '@/js/gen_process.js';

    export default {
        data() {
            return {
                iteration_display: 0,
                accuracy_display: 0,
            }
        },
        created() {
            requestAnimationFrame(this.update_values);
        },
        methods: {
            update_iteration() {
                const a = GenProcess.iteration;
                const b = this.iteration_display;
                this.iteration_display = Math.floor(a - (a - b) * 0.99);
            },
            update_accuracy() {
                const a = GenProcess.accuracy;
                const b = this.accuracy_display;
                this.accuracy_display = a - (a - b) * 0.99;
            },
            update_values() {
                this.update_iteration();
                this.update_accuracy();
                requestAnimationFrame(this.update_values);
            }
        },
        computed: {
            time() {
                return GenProcess.time.toFixed(1);
            },
            accuracy_display_formatted() {
                let res = this.accuracy_display ** 10;
                res = Math.floor(res * 10000) / 100;
                return res.toFixed(2);
            }
        }
    }
</script>
