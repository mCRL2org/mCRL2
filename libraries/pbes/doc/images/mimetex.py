import os

for file in os.listdir('.'):
    if file.endswith('.tex'):
        print file
        source = file
        target = file[:-4] + '.gif'
        os.system('mimetex -f %s -e ../html/images/%s' % (source, target))

