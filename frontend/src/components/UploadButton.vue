<template>
    <label class="ui-button upload-button" :class="{'ui-not-active': is_loading}">
        {{is_loading ? "Проверка изображения..." : "Выбрать изображение"}}
        <input v-if="!is_loading" type="file" accept="image/jpeg, image/png" @change="on_file_choose">
    </label>
</template>

<style lang="scss" scoped>
    .upload-button {
        font-size: 30px;
        input {
            display: none;
        }
    }
</style>

<script>
    import Session from '@/js/session.js';
    import UserImage from '@/js/user_image.js';

    export default {
        data() {
            return {
                file: null,
                is_loading: false,
                image_data: {
                    extension: null,
                    width: null,
                    height: null,
                    as_data_url: null,
                    as_base64: null,
                }
            };
        },
        methods: {
            reset() {
                this.file = null;
                for (let key in this.image_data)
                    this.image_data[key] = null;
                this.is_loading = false;
            },
            on_file_choose(event) {
                if (this.is_loading)
                    return;
                this.is_loading = true;
                this.file = event.target.files[0];
                this.validate_image();
            },
            validate_image() {
                const dataUrlReader = new FileReader();
                dataUrlReader.onload = (e) => {
                    const image = new Image();
                    image.onload = () => {
                        this.image_data.width = image.width;
                        this.image_data.height = image.height;

                        let w = image.width, h = image.height;
                        let mx = Math.max(w, h);
                        let MAX_SIZE = Math.max(Session.consts.edges_canvas_size, Session.consts.paint_canvas_size);
                        MAX_SIZE = Math.max(MAX_SIZE, 512);
                        let mul = (mx > MAX_SIZE) ? MAX_SIZE / mx : 1;
                        w = Math.round(w * mul);
                        h = Math.round(h * mul);

                        const canvas = document.createElement('canvas');
                        canvas.width = w;
                        canvas.height = h;
                        canvas.getContext('2d').drawImage(image, 0, 0, w, h);

                        let base64 = canvas.toDataURL("image/jpeg", 1.0);
                        this.image_data.as_base64 = base64.replace('data:image/jpeg;base64,', '');
                        this.image_data.extension = "jpg";

                        UserImage.set_data(this.image_data);
                        this.reset();
                    };
                    image.onerror = () => {
                        alert("Некорректное изображение");
                        this.reset();
                    };

                    this.image_data.as_data_url = e.target.result;
                    image.src = e.target.result;
                };
                dataUrlReader.readAsDataURL(this.file);
            },
        }
    }
</script>
