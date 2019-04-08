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
                <input-number v-model="segments" :min="0" :max="512"/>
            </div>
            <div class="config-item-text">Контурных отрезков</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <input-number v-model="triangles" :min="0" :max="128"/>
            </div>
            <div class="config-item-text">Треугольников</div>
        </div>
        <div class="config-item">
            <div class="left-column">
                <input-number v-model="circles" :min="0" :max="128"/>
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

    export default {
        components: {
            verte,
            InputNumber,
        },
        data() {
            return {
                background_color: "rgba(0, 0, 0, 0.3)",
                edges_color: "rgba(255, 255, 255, 1)",
                segments: 512,
                triangles: 50,
                circles: 0,
            };
        },
        methods: {
            launch() {
                GenProcess.start({
                    segments_cnt: this.segments,
                    background_color: this.background_color,
                    edges_color: this.edges_color
                });
            }
        }
    }
</script>
