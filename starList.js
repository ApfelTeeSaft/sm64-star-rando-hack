import { STARS } from './stars.js'

export function renderStarList(onToggle) {
	const starList = document.getElementById('starList')
	starList.innerHTML = ''

	const courseMap = {}
	const courses = []
	STARS.forEach((star) => {
		if (!courseMap[star.course]) {
			courseMap[star.course] = []
			courses.push(star.course)
		}
		courseMap[star.course].push(star)
	})

	courses.forEach((course) => {
		const group = document.createElement('div')
		group.classList.add('course-group')

		const header = document.createElement('div')
		header.classList.add('course-header')
		header.textContent = course
		group.appendChild(header)

		courseMap[course].forEach((star) => {
			const el = document.createElement('div')
			el.id = `star-${star.id}`
			el.classList.add('star-item')
			if (!star.active) el.classList.add('inactive')
			if (star.done) el.classList.add('done')

			const cbId = `cb-${star.id}`
			const cb = document.createElement('input')
			cb.type = 'checkbox'
			cb.id = cbId
			cb.checked = star.active
			cb.addEventListener('change', () => onToggle(star.id))

			const label = document.createElement('label')
			label.htmlFor = cbId
			label.textContent = star.name

			const meta = document.createElement('span')
			meta.classList.add('star-meta')

			const diffEl = document.createElement('span')
			diffEl.classList.add('star-diff')
			diffEl.dataset.diff = star.difficulty
			for (let i = 1; i <= 5; i++) {
				const dot = document.createElement('span')
				dot.textContent = '●'
				dot.classList.add('diff-dot')
				if (i <= star.difficulty) dot.classList.add('filled')
				diffEl.appendChild(dot)
			}
			meta.appendChild(diffEl)

			if (star.time > 0) {
				const timeEl = document.createElement('span')
				timeEl.classList.add('star-time')
				const totalS = Math.floor(star.time / 1000)
				const m = Math.floor(totalS / 60)
				const s = String(totalS % 60).padStart(2, '0')
				timeEl.textContent = `${m}:${s}`
				meta.appendChild(timeEl)
			}

			el.appendChild(cb)
			el.appendChild(label)
			el.appendChild(meta)
			group.appendChild(el)
		})

		starList.appendChild(group)
	})

	syncToggleAllCheckbox()
}

export function toggleStar(id) {
	const star = STARS.find((s) => s.id === id)
	if (!star) return
	star.active = !star.active
	const el = document.getElementById(`star-${id}`)
	if (el) {
		el.classList.toggle('inactive', !star.active)
		const cb = el.querySelector('input[type=checkbox]')
		if (cb) cb.checked = star.active
	}
	syncToggleAllCheckbox()
}

export function toggleAllStars(checked) {
	STARS.forEach((star) => {
		star.active = checked
		const el = document.getElementById(`star-${star.id}`)
		if (el) {
			el.classList.toggle('inactive', !checked)
			const cb = el.querySelector('input[type=checkbox]')
			if (cb) cb.checked = checked
		}
	})
	syncToggleAllCheckbox()
}

export function syncToggleAllCheckbox() {
	const cb = document.getElementById('toggleAllCheckbox')
	if (!cb) return
	const activeCount = STARS.filter((s) => s.active).length
	cb.checked = activeCount === STARS.length
	cb.indeterminate = activeCount > 0 && activeCount < STARS.length
}
