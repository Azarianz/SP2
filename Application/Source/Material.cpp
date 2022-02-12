#include "Material.h"

Material::Material()
{
	kAmbient.Set(.2f, .2f, .2f);
	kDiffuse.Set(.4f, .4f, .4f);
	kSpecular.Set(.3f, .3f, .3f);
	kShininess = 0.5;
	size = 0;
}
