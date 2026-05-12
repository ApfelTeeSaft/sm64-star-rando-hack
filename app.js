import { STARS } from './stars.js'
import { loadTemplates, saveTemplates, applyTemplate } from './templates.js'
import { isRunActive, getQueue, getTotal, startRun, endRun, pickNext } from './run.js'
import { initVoices, speak } from './speech.js'
import { renderStarList, toggleStar, toggleAllStars } from './starList.js'

// ── Template state ────────────────────────────────────────────────────────────

let templates = loadTemplates()

function renderTemplateSelect(selected) {
	const sel = document.getElementById('templateSelect')
	sel.innerHTML = ''
	Object.keys(templates).forEach((name) => {
		const opt = document.createElement('option')
		opt.value = name
		opt.textContent = name
		if (name === selected) opt.selected = true
		sel.appendChild(opt)
	})
	updateDeleteBtn()
}

function updateDeleteBtn() {
	const selected = document.getElementById('templateSelect').value
	document.getElementById('deleteTemplateBtn').disabled = selected === 'all'
}

function onSaveTemplate() {
	const input = document.getElementById('templateName')
	const name = input.value.trim()
	if (!name) {
		alert('Please enter a template name.')
		return
	}
	templates[name] = STARS.filter((s) => s.active).map((s) => s.id)
	saveTemplates(templates)
	renderTemplateSelect(name)
	input.value = ''
}

function onDeleteTemplate() {
	const name = document.getElementById('templateSelect').value
	if (name === 'all') return
	if (!confirm(`Delete template "${name}"?`)) return
	delete templates[name]
	saveTemplates(templates)
	applyTemplate(templates, 'all')
	renderStarList(toggleStar)
	renderTemplateSelect('all')
}

// ── Run UI ────────────────────────────────────────────────────────────────────

function updateRunUI() {
	const status = document.getElementById('runStatus')
	const startBtn = document.getElementById('startRunBtn')
	const endBtn = document.getElementById('endRunBtn')
	const queue = getQueue()
	if (queue !== null) {
		const picked = getTotal() - queue.length
		status.textContent = `Run in progress: ${picked} / ${getTotal()} stars picked`
		startBtn.disabled = true
		endBtn.disabled = false
	} else {
		status.textContent = ''
		startBtn.disabled = false
		endBtn.disabled = true
	}
}

function onStartRun() {
	const maxStars = parseInt(document.getElementById('runMaxStars').value, 10) || 0
	const maxDifficulty = parseInt(document.getElementById('runMaxDiff').value, 10) || 5
	if (!startRun({ maxStars, maxDifficulty })) {
		alert('No active stars match the selected options!')
		return
	}
	renderStarList(toggleStar)
	updateRunUI()
}

function onEndRun() {
	endRun()
	document.getElementById('starDisplay').textContent = ''
	renderStarList(toggleStar)
	updateRunUI()
}

// ── Pick ──────────────────────────────────────────────────────────────────────

function pickRandomStar() {
	if (isRunActive()) {
		const result = pickNext()
		if (result.type === 'complete') {
			document.getElementById('starDisplay').textContent = 'Run complete! 🎉'
			speak('Run complete! Congratulations!', document.getElementById('voices').value)
			updateRunUI()
			return
		}
		const { star } = result
		const el = document.getElementById(`star-${star.id}`)
		if (el) el.classList.add('done')
		document.getElementById('starDisplay').textContent = `${star.course}: ${star.name}`
		speak(`${star.course}: ${star.name}`, document.getElementById('voices').value)
		updateRunUI()
		return
	}
	const active = STARS.filter((s) => s.active)
	if (active.length === 0) {
		alert('No active stars to pick from!')
		return
	}
	const star = active[Math.floor(Math.random() * active.length)]
	document.getElementById('starDisplay').textContent = `${star.course}: ${star.name}`
	speak(`${star.course}: ${star.name}`, document.getElementById('voices').value)
}

// ── Init ──────────────────────────────────────────────────────────────────────

renderTemplateSelect()
renderStarList(toggleStar)
initVoices(document.getElementById('voices'))
updateRunUI()

document.getElementById('templateSelect').addEventListener('change', (e) => {
	applyTemplate(templates, e.target.value)
	renderStarList(toggleStar)
	updateDeleteBtn()
})
document.getElementById('deleteTemplateBtn').addEventListener('click', onDeleteTemplate)
document.getElementById('saveTemplateBtn').addEventListener('click', onSaveTemplate)
document.getElementById('toggleAllCheckbox').addEventListener('change', (e) => {
	toggleAllStars(e.target.checked)
})
document.getElementById('startRunBtn').addEventListener('click', onStartRun)
document.getElementById('endRunBtn').addEventListener('click', onEndRun)
document.getElementById('pickStarBtn').addEventListener('click', pickRandomStar)
document.addEventListener('keydown', (e) => {
	if (e.code === 'Space') {
		if (document.activeElement.tagName.toLowerCase() === 'input') return
		e.preventDefault()
		pickRandomStar()
	}
})
