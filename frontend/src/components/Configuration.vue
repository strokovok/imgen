<template>
    <div class="ui-window">
        <div class="config-item">
            <div class="left-column">
                <verte class="color-picker" v-model="background_color" model="rgb" :recentColors="null"/>
            </div>
            <div class="config-item-text">Цвет фона</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <verte class="color-picker" v-model="edges_color" model="rgb" :recentColors="null"/>
            </div>
            <div class="config-item-text">Цвет контура</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <input-number v-model="segments" :min="0" :max="consts.max_segments"/>
            </div>
            <div class="config-item-text">Контурных отрезков (0 - {{consts.max_segments}})</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <input-number v-model="axis_div" :min="2" :max="15"/>
            </div>
            <div class="config-item-text">Детализация окраски (2 - {{consts.max_axis_div}})</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <input-number v-model="paint_opacity" :min="0" :max="100"/>
            </div>
            <div class="config-item-text">Непрозрачность окраски (0 - 100)</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <input-number v-model="triangles" :min="0" :max="consts.max_triangles"/>
            </div>
            <div class="config-item-text">Треугольников (0 - {{consts.max_triangles}})</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <input-number v-model="circles" :min="0" :max="64"/>
            </div>
            <div class="config-item-text">Окружностей</div>
        </div>
        <div class="ui-button launch-button" @click="launch">Запуск</div>
    </div>
</template>

<style lang="scss" scoped>
    .config-item {
        font-weight: 200;
        font-size: 25px;
        &:not(:last-child) {
            margin-bottom: 20px;
        }
        display: flex;
        align-items: center;
    }

    .left-column {
        width: 100px;
        display: flex;
        justify-content: center;
    }

    .color-picker {
        border: 1px white solid;
        border-radius: 12px;
        background: white;
    }

    .config-item-text {
        padding-left: 0.5em;
        margin-top: -2px;
    }

    .launch-button {
        margin-top: 0.5em;
        margin-bottom: 0.1em;
        font-size: 25px;
        align-self: center;
        padding: 0.3em 5em;
    }

</style>

<script>
    import verte from 'verte';
    import 'verte/dist/verte.css';
    import InputNumber from '@/components/InputNumber.vue';
    import GenProcess from '@/js/gen_process.js';
    import Session from '@/js/session.js';

    export default {
        components: {
            verte,
            InputNumber,
        },
        data() {
            return {
                background_color: "rgba(0, 0, 0, 0.7)",
                edges_color: "rgba(0, 0, 0, 1)",
                segments: 1024,
                triangles: 1024,
                circles: 0,
                axis_div: 7,
                paint_opacity: 50,
            };
        },
        methods: {
            launch() {
                GenProcess.start({
                    segments_cnt: this.segments,
                    background_color: this.background_color,
                    edges_color: this.edges_color,
                    triangles_cnt: this.triangles,
                    circles_cnt: this.circles,
                    axis_div: this.axis_div,
                    paint_opacity: this.paint_opacity / 100,
                });
            }
        },
        computed: {
            consts() {
                return Session.consts;
            }
        }
    }
</script>
