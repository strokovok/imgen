module.exports = {
    devServer: {
        proxy: {
            '^/ws': {
                target: 'ws://0.0.0.0:40080',
                ws: true,
                changeOrigin: true,
            },
        },
    },
};
