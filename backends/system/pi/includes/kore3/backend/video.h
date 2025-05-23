#include <Kore/Graphics4/Texture.h>
#include <Kore/IO/FileReader.h>

namespace Kore {
	class VideoSoundStream;

	class Video {
	public:
		Video(const char *filename) {
			duration = 1000 * 10;
			position = 0;
			finished = false;
			paused   = false;
			image    = new Graphics4::Texture(100, 100, Graphics4::Image::RGBA32, false);
		}
		~Video() {
			delete image;
		}
		void play() {}
		void pause() {}
		void stop() {}
		int  width() {
            return 100;
		}
		int height() {
			return 100;
		}
		Graphics4::Texture *currentImage() {
			return image;
		}
		double duration; // milliseconds
		double position; // milliseconds
		bool   finished;
		bool   paused;
		void   update(double time) {}

	private:
		Graphics4::Texture *image;
	};
}
