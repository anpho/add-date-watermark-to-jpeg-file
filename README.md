# add-date-watermark-to-jpeg-file

This command line tool is used to add DATE watermark to JPEG files.

## How to install 

Nope. no installer provided.

## How to use

Open your commandline, assume your images are in `D:/myphoto/` directory, and you want to add Date watermark to them, placing at `D:/watermarked-photos` .

```
for %a in (D:\myphoto\*.jpg) do add-watermark-by-exif.exe %a d:\watermarked-photos
```

## Contact

Merrick Zhang (anphorea@gmail.com)