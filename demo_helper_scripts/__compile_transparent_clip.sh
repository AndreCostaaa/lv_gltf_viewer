clear
rm ../output_transparent_clip.webm
ffmpeg -framerate 30 -i ../render_frames/frame%03d.png -c:v libvpx -pix_fmt yuva420p -auto-alt-ref 0 -b:v 2M ../output_transparent_clip.webm
vlc ../output_transparent_clip.webm