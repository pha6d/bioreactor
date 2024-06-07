import { createRouter, createWebHistory } from 'vue-router'
import BioreactorData from '../components/BioreactorData.vue'

const routes = [
    {
        path: '/',
        name: 'BioreactorData',
        component: BioreactorData
    }
]

const router = createRouter({
    history: createWebHistory(process.env.BASE_URL),
    routes
})

export default router
