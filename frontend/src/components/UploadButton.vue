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
                    as_array_buffer: null,
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
            on_load() {
                if (!this.image_data.as_data_url || !this.image_data.as_array_buffer)
                    return;
                UserImage.set_data(this.image_data);
                this.reset();
            },
            on_valid_image() {
                const dataUrlReader = new FileReader();
                dataUrlReader.onload = (e) => {
                    this.image_data.as_data_url = e.target.result;
                    this.on_load();
                };
                dataUrlReader.readAsDataURL(this.file);

                const binaryReader = new FileReader();
                binaryReader.onload = (e) => {
                    this.image_data.as_array_buffer = e.target.result;
                    this.on_load();
                };
                binaryReader.readAsArrayBuffer(this.file);
            },
            on_invalid_image() {
                alert("Некорректное изображение");
                this.reset();
            },
            validate_image() {
                if (this.file.size > (10 * 1024 * 1024)) {
                    alert("Файл слишком большой (>10мб)");
                    this.reset();
                }

                this.image_data.extension = this.file.type.replace("image/", "");

                let image = new Image();
                image.onload = () => {
                    if (image.width && image.height) {
                        this.image_data.width = image.width;
                        this.image_data.height = image.height;
                        this.on_valid_image();
                    } else
                        this.on_invalid_image();
                };
                image.onerror = () => {
                    this.on_invalid_image();
                };
                let url = window.URL || window.webkitURL;
                image.src = url.createObjectURL(this.file);
            },
        }
    }
</script>
