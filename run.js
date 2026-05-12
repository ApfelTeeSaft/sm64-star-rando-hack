import { STARS } from './stars.js'

let queue = null
let total = 0

function shuffle(arr) {
	for (let i = arr.length - 1; i > 0; i--) {
		const j = Math.floor(Math.random() * (i + 1))
		;[arr[i], arr[j]] = [arr[j], arr[i]]
	}
}

export function isRunActive() {
	return queue !== null
}

export function getQueue() {
	return queue
}

export function getTotal() {
	return total
}

export function startRun({ maxStars = 0, maxDifficulty = 5 } = {}) {
	let active = STARS.filter((s) => s.active)
	if (maxDifficulty < 5) active = active.filter((s) => s.difficulty <= maxDifficulty)
	if (active.length === 0) return false
	STARS.forEach((s) => (s.done = false))
	queue = [...active]
	shuffle(queue)
	if (maxStars > 0 && maxStars < queue.length) queue = queue.slice(0, maxStars)
	total = queue.length
	return true
}

export function endRun() {
	queue = null
	total = 0
	STARS.forEach((s) => (s.done = false))
}

export function pickNext() {
	if (queue === null) return { type: 'no-run' }
	if (queue.length === 0) return { type: 'complete' }
	const star = queue.pop()
	star.done = true
	return { type: 'picked', star }
}
