import { STARS } from './stars.js'

const STORAGE_KEY = 'sm64-templates'

export function loadTemplates() {
	const defaults = { all: STARS.map((s) => s.id) }
	const stored = localStorage.getItem(STORAGE_KEY)
	if (!stored) return defaults
	try {
		const parsed = JSON.parse(stored)
		parsed.all = defaults.all
		return parsed
	} catch {
		return defaults
	}
}

export function saveTemplates(templates) {
	localStorage.setItem(STORAGE_KEY, JSON.stringify(templates))
}

export function applyTemplate(templates, name) {
	const ids = templates[name]
	STARS.forEach((s) => (s.active = ids.includes(s.id)))
}
