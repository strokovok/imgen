<template>
    <div class="container">
        <img :src="img_src" class="user-image user-image-bg" :style="img_size_style">
        <div class="crop-container" :style="crop_style">
            <img :src="img_src" class="user-image" :style="img_size_style">
        </div>
    </div>
</template>

<style lang="scss" scoped>
    .container {
        position: relative;
    }

    .user-image {
        border-radius: 4px;
    }

    .user-image-bg {
        opacity: 0.3;
    }

    .crop-container {
        height: 100%;
        overflow: hidden;
        position: absolute;
        top: 0;
        left: 0;
    }

</style>

<script>
    import UserImage from '@/js/user_image.js';

    const MAX_WIDTH = 640;
    const MAX_HEIGHT = 640;

    export default {
        computed: {
            img_src() {
                return UserImage.image_data.as_data_url;
            },
            img_size_style() {
                let width = UserImage.image_data.width;
                let height = UserImage.image_data.height;
                let mul = (width > height) ? MAX_WIDTH / width : MAX_HEIGHT / height;
                width *= mul;
                height *= mul;
                return {
                    "width": `${width}px`,
                    "height": `${height}px`,
                }
            },
            crop_style() {
                const percents = UserImage.uploading_progress * 100;
                return {
                    "width": `${percents}%`,
                }
            }
        }
    }
</script>
