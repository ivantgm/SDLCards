def my_resize(filename, w, h):
        pdb.gimp_file_load('/home/ivantgm/Área de Trabalho/SDL/cards-img-set/'+filename, '?')
        img = gimp.image_list()[0]
        pdb.gimp_context_set_interpolation(INTERPOLATION_LANCZOS)
        pdb.gimp_image_scale(img, w, h)
        pdb.gimp_file_save(img,img.layers[0],'/home/ivantgm/Área de Trabalho/SDL/cards-img-set/'+str(w)+'x'+str(h)+'/'+filename, '?')
        
        
images = ['000.png','011.png','012.png','013.png','014.png','021.png','022.png','023.png','024.png','031.png','032.png','033.png','034.png','041.png','042.png','043.png','044.png','051.png','052.png','053.png','054.png','061.png','062.png','063.png','064.png','071.png','072.png','073.png','074.png','081.png','082.png','083.png','084.png','091.png','092.png','093.png','094.png','101.png','102.png','103.png','104.png','111.png','112.png','113.png','114.png','121.png','122.png','123.png','124.png','131.png','132.png','133.png','134.png']


for i in images:
        my_resize(i, 100, 150)
        
        
        

