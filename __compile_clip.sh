clear
rm ./output_clip.mp4
ffmpeg -framerate 30 -i render_frames/frame%03d.png -c:v libx264 -pix_fmt yuv420p -b:v 2M output_clip.mp4
vlc ./output_clip.mp4