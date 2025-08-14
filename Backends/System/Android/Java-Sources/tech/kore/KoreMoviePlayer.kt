package tech.kore

import java.util.ArrayList

import android.view.Surface

class KoreMoviePlayer(var path: String) {
	companion object {
		var players = ArrayList<KoreMoviePlayer?>()

		@JvmStatic
		fun updateAll() {
			for (player in KoreMoviePlayer.players) {
				player!!.update()
			}
		}

		fun remove(id: Int) {
			players[id] = null
		}
	}

	private var movieTexture: KoreMovieTexture? = null
	var id: Int = players.size

	init {
		players.add(this)
	}
	
	fun init() {
		movieTexture = KoreMovieTexture()
		val surface = Surface(movieTexture!!.surfaceTexture)
		nativeCreate(path, surface, id)
		surface.release()
	}

	fun getMovieTexture(): KoreMovieTexture? {
		return movieTexture
	}

	fun update(): Boolean {
		return movieTexture!!.update()
	}
	
	fun getTextureId(): Int {
		return movieTexture!!.textureId
	}

	private external fun nativeCreate(path: String, surface: Surface, id: Int)
}
