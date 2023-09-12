import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react-swc'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    proxy: {
      "/api": {
        target: "http://santi-server.local",
        changeOrigin: true,
      },
      "/ws": {
        target: "http://santi-server.local",
        ws: true,
      },
    },
  },
});
