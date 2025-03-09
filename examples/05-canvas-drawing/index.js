import sdl from '@kmamal/sdl'
import Canvas from '@napi-rs/canvas'

const window = sdl.video.createWindow({ resizable: true })

let canvas
let ctx

const redraw = () => {
	const { pixelWidth: width, pixelHeight: height } = window

	ctx.font = `${Math.floor(height / 5)}px serif`
	ctx.fillStyle = 'red'
	ctx.textAlign = 'center'
	ctx.fillText("Hello, World!", width / 2, height / 2)

	const buffer = Buffer.from(ctx.getImageData(0, 0, width, height).data)
	window.render(width, height, width * 4, 'rgba32', buffer)
}

window.on('expose', redraw)

window.on('resize', (event) => {
	canvas = Canvas.createCanvas(event.pixelWidth, event.pixelHeight)
	ctx = canvas.getContext('2d')
	redraw()
})
