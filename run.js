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

function weightedSample(arr, weights) {
	// Exclude difficulties with weight 0
	const eligible = arr.filter((s) => weights[s.difficulty - 1] > 0)
	// Assign each star a score: U^(1/w) — higher weight → higher expected score
	return eligible
		.map((s) => ({ s, key: Math.random() ** (1 / weights[s.difficulty - 1]) }))
		.sort((a, b) => b.key - a.key)
		.map(({ s }) => s)
}

export function startRun({ maxStars = 0, orderByStage = false, difficultyWeights = [5, 5, 5, 5, 5] } = {}) {
	let active = STARS.filter((s) => s.active)
	if (active.length === 0) return false
	STARS.forEach((s) => (s.done = false))
	queue = weightedSample(active, difficultyWeights)
	if (maxStars > 0 && maxStars < queue.length) queue = queue.slice(0, maxStars)
	if (queue.length === 0) return false
	if (orderByStage) {
		queue.sort((a, b) => b.id - a.id)
	}
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
