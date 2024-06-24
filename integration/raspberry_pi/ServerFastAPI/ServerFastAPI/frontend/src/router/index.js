import { createRouter, createWebHistory } from 'vue-router'
import HomePage from '../components/HomePage.vue'
import PIDPage from '../components/PIDPage.vue'
import ChartsPage from '../components/ChartsPage.vue'
import ProgramsPage from '../components/ProgramsPage.vue'
import SettingsPage from '../components/SettingsPage.vue'
import CalibrationPage from '../components/CalibrationPage.vue'

const routes = [
  { path: '/', component: HomePage, name: 'Home' },
  { path: '/pid', component: PIDPage, name: 'PID' },
  { path: '/charts', component: ChartsPage, name: 'Charts' },
  { path: '/programs', component: ProgramsPage, name: 'Programs' },
  { path: '/settings', component: SettingsPage, name: 'Settings' },
  { path: '/calibration', component: CalibrationPage, name: 'Calibration' }
]

const router = createRouter({
  history: createWebHistory(),
  routes
})

export default router
