/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Michal Pyšík, xpysik00@fit.vutbr.cz
 */

#include <student/gpu.hpp>


/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
	binded_vao = NULL;
	binded_prg = NULL;
	binded_fb = NULL;
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
	if (binded_fb != NULL) deleteFramebuffer();
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>

	auto buf_ptr = std::make_unique<uint8_t[]>(size);
	auto buf_ID = reinterpret_cast<BufferID>(buf_ptr.get());
	buffers.push_back(std::move(buf_ptr));

	return buf_ID;
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.

	if (!isBuffer(buffer)) return;

	for (auto& selected_buffer : buffers)
	{
		if (selected_buffer.get() == reinterpret_cast<uint8_t*>(buffer)) selected_buffer.release();
	}

	
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>

	if (!isBuffer(buffer)) return;

	auto subject = reinterpret_cast<uint8_t*>(buffer) + offset;
	std::copy((uint8_t*)data, (uint8_t*)data + size, subject);
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>

	if (!isBuffer(buffer)) return;

	auto subject = reinterpret_cast<uint8_t*>(buffer) + offset;
	std::copy(subject, subject + size, (uint8_t*)data);
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) {
	/// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
	/// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
	/// Pro emptyId vrací false.<br>

	if (buffer == emptyID) return false;

	for (auto& selected_buffer : buffers)
	{
		if (selected_buffer.get() == reinterpret_cast<uint8_t*>(buffer)) return true;
	}

  return false; 
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller     (){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.

	std::unique_ptr<vTable> newTable = std::make_unique<vTable>();
	newTable->indexBuffer = emptyID;

	for (int i = 0; i < maxAttributes; i++)
	{
		newTable->heads[i].enabled = false;
	}

	newTable->isIndexing = false;

	auto table_ID = reinterpret_cast<ObjectID>(newTable.get());
	pullerTables.push_back(std::move(newTable));

	return table_ID;

	
  //return emptyID;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void     GPU::deleteVertexPuller     (VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>

	if (!isVertexPuller(vao)) return;

	for (auto& selected_table : pullerTables)
	{
		if (selected_table.get() == reinterpret_cast<vTable*>(vao)) selected_table.release();
	}
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void     GPU::setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>

	if (vao == emptyID) return;

	for (auto& selected_table : pullerTables)
	{
		if (selected_table.get() == reinterpret_cast<vTable*>(vao))
		{
			selected_table->heads[head].hBuffer = buffer;
			selected_table->heads[head].hStride = stride;
			selected_table->heads[head].hOffset = offset;
			selected_table->heads[head].aType = type;
		}
	}
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void     GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>

	if (vao == emptyID) return;

	for (auto& selected_table : pullerTables)
	{
		if (selected_table.get() == reinterpret_cast<vTable*>(vao))
		{
			selected_table->isIndexing = true;
			selected_table->iType = type;
			selected_table->indexBuffer = buffer;
		}
	}
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void     GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>

	if (vao == emptyID) return;

	for (auto& selected_table : pullerTables)
	{
		if (selected_table.get() == reinterpret_cast<vTable*>(vao))
		{
			selected_table->heads[head].enabled = true;
		}
	}
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void     GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>

	if (vao == emptyID) return;

	for (auto& selected_table : pullerTables)
	{
		if (selected_table.get() == reinterpret_cast<vTable*>(vao))
		{
			selected_table->heads[head].enabled = false;
		}
	}
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void     GPU::bindVertexPuller       (VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
	if (!isVertexPuller(vao)) return;
	else
	{
		for (auto& selected_table : pullerTables)
		{
			if (selected_table.get() == reinterpret_cast<vTable*>(vao)) binded_vao=selected_table.get();
		}
	}
}

/**
 * @brief This function deactivates vertex puller.
 */
void     GPU::unbindVertexPuller     (){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.

	binded_vao = NULL;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool     GPU::isVertexPuller         (VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.

	if (vao == emptyID) return false;

	for (auto& selected_table : pullerTables)
	{
		
		if (selected_table.get() == reinterpret_cast<vTable*>(vao)) return true;
	}
	
	return false;

}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID        GPU::createProgram         (){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>

	std::unique_ptr<progSettings> newPRG = std::make_unique<progSettings>();

	for (int i = 0; i < maxAttributes;i++)
	{
		newPRG->progVFAT[i] = AttributeType::EMPTY;
	}
	auto prg_ID = reinterpret_cast<ProgramID>(newPRG.get());
	prgArray.push_back(std::move(newPRG));

	return prg_ID;

  //return emptyID;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void             GPU::deleteProgram         (ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>

	if (!isProgram(prg)) return;

	for (auto& selected_prg : prgArray)
	{
		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg)) selected_prg.release();
	}
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void             GPU::attachShaders         (ProgramID prg,VertexShader vs,FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.

	if (prg == emptyID) return;

	for (auto& selected_prg : prgArray)
	{
		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg))
		{
			selected_prg->progVS = vs;
			selected_prg->progFS = fs;
		}
	}
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void             GPU::setVS2FSType          (ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>

	if (prg == emptyID) return;

	for (auto& selected_prg : prgArray)
	{
		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg))
		{
			selected_prg->progVFAT[attrib] = type;
		}
	}
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void             GPU::useProgram            (ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
	if (!isProgram(prg)) return;
	else
	{
		for (auto& selected_prg : prgArray)
		{
			if (selected_prg.get() == reinterpret_cast<progSettings*>(prg)) binded_prg = selected_prg.get();
		}
	}


}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool             GPU::isProgram             (ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>

	if (prg == emptyID) return false;

	for (auto& selected_prg : prgArray)
	{

		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg)) return true;
	}

	return false;
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform1f      (ProgramID prg,uint32_t uniformId,float     const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>

	if (prg == emptyID) return;

	for (auto& selected_prg : prgArray)
	{

		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg))
		{
			selected_prg->progUni.uniform[uniformId].v1 = d;
		}
	}

}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform2f      (ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.

	if (prg == emptyID) return;

	for (auto& selected_prg : prgArray)
	{

		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg))
		{
			selected_prg->progUni.uniform[uniformId].v2 = d;
		}
	}
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform3f      (ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.

	if (prg == emptyID) return;

	for (auto& selected_prg : prgArray)
	{

		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg))
		{
			selected_prg->progUni.uniform[uniformId].v3 = d;
		}
	}
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniform4f      (ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.

	if (prg == emptyID) return;

	for (auto& selected_prg : prgArray)
	{

		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg))
		{
			selected_prg->progUni.uniform[uniformId].v4 = d;
		}
	}
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void             GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).

	if (prg == emptyID) return;

	for (auto& selected_prg : prgArray)
	{

		if (selected_prg.get() == reinterpret_cast<progSettings*>(prg))
		{
			selected_prg->progUni.uniform[uniformId].m4 = d;
		}
	}
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer      (uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>
	if (binded_fb != NULL) deleteFramebuffer();

	binded_fb = new FrameBuffer;
	binded_fb->fbHeight = height;
	binded_fb->fbWidth = width;
	
	binded_fb->cPixels = new uint8_t[sizeof(ColorPixel)*height*width];
	binded_fb->dPixels = new float[height*width];
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer      (){
  /// \todo tato funkce by měla dealokovat framebuffer.

	delete [] binded_fb->cPixels;
	delete [] binded_fb->dPixels;
	delete binded_fb;
	binded_fb = NULL;
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void     GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.

	GPU::deleteFramebuffer();
	GPU::createFramebuffer( width, height );
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor  (){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
  if(GPU::binded_fb == NULL) return nullptr;
 
  uint8_t *tmp = GPU::binded_fb->cPixels;
  return tmp;
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth    (){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
	if (GPU::binded_fb == NULL) return nullptr;

	float *tmp = GPU::binded_fb->dPixels;
	return tmp;

}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth (){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
	if (binded_fb != NULL) return binded_fb->fbWidth;
  return 0;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
	if (binded_fb != NULL) return binded_fb->fbHeight;
	return 0;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void            GPU::clear                 (float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
	if (binded_fb == NULL) return;

	int current_byte_size = 4*(getFramebufferHeight())*(getFramebufferWidth());
	
	uint8_t red = round(255 * r);  //neni treba podminka, kdyz argumenty presahnou rozmezi <0,1> tak to proste pretece, ale program nespadne a barvy se vykresli
	uint8_t green = round(255 * g);
	uint8_t blue = round(255 * b);
	uint8_t alpha = round(255 * a);

	for (int i = 0; i < current_byte_size; i+=4)
	{
		binded_fb->cPixels[i] = red;
		binded_fb->cPixels[i+1] = green;
		binded_fb->cPixels[i+2] = blue;
		binded_fb->cPixels[i+3] = alpha;
		binded_fb->dPixels[i/4] = 1.25;
	}

}


// Funkce vykreslí nový pixel v barevném i hloubkovém bufferu
// pPositin = x, y, z koordinaty
//pColor = red, green, blue, alpha hodnoty v rozmezi <0,1>
void			GPU::putPixel(glm::vec3 pPosition, glm::vec4 pColor, uint32_t swidth, uint32_t sheight)
{
	int dIndex = pPosition.x + swidth * pPosition.y;
	if (dIndex > sheight*swidth) return; //neopravneny zasah do pameti
	int cIndex = dIndex * 4; // barevne pole ve framebufferu ma 4x vice 8bit cisel nez hloubkove pole floatu

	for (int i = 0; i <= 3; i++)
	{
		if (pColor[i] > 1.0) pColor[i] = 1.0;
		else if (pColor[i] < 0.0) pColor[i] = 0.0;
	}

	binded_fb->dPixels[dIndex] = pPosition.z;

	uint8_t red = round(255 * pColor[0]);
	uint8_t green = round(255 * pColor[1]);
	uint8_t blue = round(255 * pColor[2]);
	uint8_t alpha = round(255 * pColor[3]);

	binded_fb->cPixels[cIndex] = red;
	binded_fb->cPixels[cIndex + 1] = green;
	binded_fb->cPixels[cIndex + 2] = blue;
	binded_fb->cPixels[cIndex + 3] = alpha;
}

//zjisti zda je hloubka noveho pixelu mensi nez pixelu co jiz v frabebufferu je
bool		   GPU::isPixelCloser(glm::vec3 pPosition, uint32_t swidth, uint32_t sheight)
{
	int dIndex = pPosition.x + swidth * pPosition.y;
	if (dIndex > sheight*swidth) return false; //neopravneny zasah do pameti

	if (pPosition.z < binded_fb->dPixels[dIndex]) return true;
	else return false;
}



float GPU::getMax(float a,float b)
{
	if (a > b) return a;
	else return b;
}


float GPU::getMin(float a,float b)
{
	if (a < b) return a;
	else return b;
}



//Zazrak od Pana Pinedy
float GPU::edgeFunction(const glm::vec4 &a, const glm::vec4 &b, const glm::vec4 &c)
{
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}


float GPU::InterpolateAttribute(float lam0, float lam1, float lam2, float h0, float h1, float h2, float A0, float A1, float A2)
{
	return (((A0 * lam0) / h0) + ((A1 * lam1) / h1) + ((A2 * lam2) / h2)) / ((lam0 / h0) + (lam1 / h1) + (lam2 / h2));
}


void GPU::RasterizeTriangle(primiTriangle &triangle, const uint32_t width, const uint32_t height)
{

	int max_x = floor(getMax(getMax(triangle.tV1.gl_Position.x, triangle.tV2.gl_Position.x), triangle.tV3.gl_Position.x));
	int max_y = floor(getMax(getMax(triangle.tV1.gl_Position.y, triangle.tV2.gl_Position.y), triangle.tV3.gl_Position.y));
	int min_x = floor(getMin(getMin(triangle.tV1.gl_Position.x, triangle.tV2.gl_Position.x), triangle.tV3.gl_Position.x));
	int min_y = floor(getMin(getMin(triangle.tV1.gl_Position.y, triangle.tV2.gl_Position.y), triangle.tV3.gl_Position.y));

	float bc1, bc2, bc3;
	bc1 = bc2 = bc3 = 6.9;
	float hr1, hr2, hr3;
	hr1 = hr2 = hr3 = 6.9;
	int xhelp = min_x;
	int yhelp = min_y;
	float area = edgeFunction(triangle.tV1.gl_Position, triangle.tV2.gl_Position, triangle.tV3.gl_Position);
	glm::vec3 deltaX;
	glm::vec3 deltaY;

	deltaX[0] = triangle.tV3.gl_Position.x - triangle.tV2.gl_Position.x;
	deltaX[1] = triangle.tV1.gl_Position.x - triangle.tV3.gl_Position.x;
	deltaX[2] = triangle.tV2.gl_Position.x - triangle.tV1.gl_Position.x;
	deltaY[0] = triangle.tV3.gl_Position.y - triangle.tV2.gl_Position.y;
	deltaY[1] = triangle.tV1.gl_Position.y - triangle.tV3.gl_Position.y;
	deltaY[2] = triangle.tV2.gl_Position.y - triangle.tV1.gl_Position.y; //v jednom radku me to nenecha nahrat, z neznameho duvodu se treti prvek vynuluje tak to mam ted pro jistotu takto


	for (int x = min_x; x <= max_x; x++)
	{
		for (int y = min_y; y <= max_y; y++)
		{
			if (x == min_x && y == min_y)
			{
				hr1 = edgeFunction(triangle.tV2.gl_Position, triangle.tV3.gl_Position, glm::vec4(x + 0.5f, y + 0.5f, 1, 1)); //cyklim pres for kde jsou jen integery takze 0.5 pro stred pixelu pricitam az tu
				hr2 = edgeFunction(triangle.tV3.gl_Position, triangle.tV1.gl_Position, glm::vec4(x + 0.5f, y + 0.5f, 1, 1));
				hr3 = edgeFunction(triangle.tV1.gl_Position, triangle.tV2.gl_Position, glm::vec4(x + 0.5f, y + 0.5f, 1, 1));
			}
			else
			{
				hr1 = hr1 + ( (x - xhelp) * deltaY[0] - (y - yhelp) * deltaX[0] );
				hr2 = hr2 + ( (x - xhelp) * deltaY[1] - (y - yhelp) * deltaX[1] );
				hr3 = hr3 + ( (x - xhelp) * deltaY[2] - (y - yhelp) * deltaX[2] );
				
				xhelp = x;
				yhelp = y;
			}

			//pokud je bod uvnitr trojuhelniku tak jdem interpolovat atributy a rovnou ho zapsat do framebufferu (misto swapovani kontroluji obracenou orientaci)
			if ((hr1 >= 0 && hr2 >= 0 && hr3 >= 0) || (hr1 <= 0 && hr2 <= 0 && hr3 <= 0))
			{
				//zdroj pro barycentricke coord. primo z edge funkce: https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
				bc1 = hr1 / area;
				bc2 = hr2 / area;
				bc3 = hr3 / area;

				InFragment ifrg;
				ifrg.gl_FragCoord.x = x+0.5f; //x cyklim pres for cyklus, takze music 0.5 pricist
				ifrg.gl_FragCoord.y = y+0.5f;

				//ifrg.gl_FragCoord.z = bc1 * triangle.tV1.gl_Position.z + bc2 * triangle.tV2.gl_Position.z + bc3 * triangle.tV3.gl_Position.z;
				//ifrg.gl_FragCoord.w = bc1 * triangle.tV1.gl_Position.w + bc2 * triangle.tV2.gl_Position.w + bc3 * triangle.tV3.gl_Position.w;
				ifrg.gl_FragCoord.z = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.gl_Position.z, triangle.tV2.gl_Position.z, triangle.tV3.gl_Position.z);
				ifrg.gl_FragCoord.w = (bc1 + bc2 + bc3) / (bc1 / triangle.tV1.gl_Position.w + bc2 / triangle.tV2.gl_Position.w + bc3 / triangle.tV3.gl_Position.w);

				for (int i = 0; i < maxAttributes; i++)
				{
					if (binded_prg->progVFAT[i] == AttributeType::FLOAT)
					{
						ifrg.attributes[i].v1 = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v1, triangle.tV2.attributes[i].v1, triangle.tV3.attributes[i].v1);
					}
					else if (binded_prg->progVFAT[i] == AttributeType::EMPTY) continue;
					else if (binded_prg->progVFAT[i] == AttributeType::VEC2)
					{
						ifrg.attributes[i].v2[0] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v2[0], triangle.tV2.attributes[i].v2[0], triangle.tV3.attributes[i].v2[0]);
						ifrg.attributes[i].v2[1] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v2[1], triangle.tV2.attributes[i].v2[1], triangle.tV3.attributes[i].v2[1]);
					}
					else if (binded_prg->progVFAT[i] == AttributeType::VEC3)
					{
						ifrg.attributes[i].v3[0] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v3[0], triangle.tV2.attributes[i].v3[0], triangle.tV3.attributes[i].v3[0]);
						ifrg.attributes[i].v3[1] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v3[1], triangle.tV2.attributes[i].v3[1], triangle.tV3.attributes[i].v3[1]);
						ifrg.attributes[i].v3[2] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v3[2], triangle.tV2.attributes[i].v3[2], triangle.tV3.attributes[i].v3[2]);
					}
					else if (binded_prg->progVFAT[i] == AttributeType::VEC4)
					{
						ifrg.attributes[i].v4[0] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v4[0], triangle.tV2.attributes[i].v4[0], triangle.tV3.attributes[i].v4[0]);
						ifrg.attributes[i].v4[1] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v4[1], triangle.tV2.attributes[i].v4[1], triangle.tV3.attributes[i].v4[1]);
						ifrg.attributes[i].v4[2] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v4[2], triangle.tV2.attributes[i].v4[2], triangle.tV3.attributes[i].v4[2]);
						ifrg.attributes[i].v4[3] = InterpolateAttribute(bc1, bc2, bc3, triangle.tV1.gl_Position.w, triangle.tV2.gl_Position.w, triangle.tV3.gl_Position.w, triangle.tV1.attributes[i].v4[3], triangle.tV2.attributes[i].v4[3], triangle.tV3.attributes[i].v4[3]);
					}
					
				}

				OutFragment ofrg;

				binded_prg->progFS(ofrg, ifrg, binded_prg->progUni);

				if (isPixelCloser(glm::vec3(x, y, ifrg.gl_FragCoord.z), width, height))
				{
					putPixel(glm::vec3(x, y, ifrg.gl_FragCoord.z), ofrg.gl_FragColor, width, height);
				}
			}
		}
	}

	return;
}



void            GPU::drawTriangles         (uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>
	if (binded_fb == NULL || binded_prg == NULL || binded_vao == NULL) return;

	uint32_t const screenWidth = getFramebufferWidth();
	uint32_t const screenHeight = getFramebufferHeight();

	uint64_t sz;
	uint64_t offs;

	InVertex *inV = new InVertex[nofVertices];
	OutVertex *outV = new OutVertex[nofVertices];

/*	glm::mat4 viewportMatrix = glm::mat4(0.5*screenWidth, 0, 0, 1.0f*0.5*screenWidth,  <- funguje jen kdyz homogenni slozku nastavim na 1 (jinak se posun nasobi vyssim cislem)
		0, 0.5*screenHeight, 0, 1.0f*0.5*screenHeight,
		0, 0, 1, 0,
		0, 0, 0, 1);
*/
	for (uint32_t j = 0; j < nofVertices; j++)
	{
		inV[j].gl_VertexID = 0;          //inicializace nutna pro pripad 16bit nebo 8bit indexovani, a hodi se i jindy, jistota je jistota
	}



	std::vector<primiTriangle> pTriangles; //dynamicke pole trojuhelniku


	for (uint32_t i = 0; i < nofVertices; i++)
	{
		if (binded_vao->isIndexing)
		{
			sz = static_cast<uint64_t>(binded_vao->iType);
			offs = static_cast<uint64_t>(i * sz);

			getBufferData(binded_vao->indexBuffer,
				offs,
				sz,
				&(inV[i].gl_VertexID));
		}
		else inV[i].gl_VertexID = i;

		for (uint32_t a = 0; a < maxAttributes; a++)
		{
			if (binded_vao->heads[a].enabled)
			{
				if (binded_vao->heads[a].aType == AttributeType::FLOAT)
				{
					getBufferData(binded_vao->heads[a].hBuffer,
						(binded_vao->heads[a].hOffset) + inV[i].gl_VertexID * (binded_vao->heads[a].hStride),
						sizeof(float),
						&inV[i].attributes[a].v1);
				}
				else if (binded_vao->heads[a].aType == AttributeType::VEC2)
				{
					getBufferData(binded_vao->heads[a].hBuffer,
						(binded_vao->heads[a].hOffset) + inV[i].gl_VertexID * (binded_vao->heads[a].hStride),
						2*sizeof(float),
						&inV[i].attributes[a].v2);
				}
				if (binded_vao->heads[a].aType == AttributeType::VEC3)
				{
					getBufferData(binded_vao->heads[a].hBuffer,
						(binded_vao->heads[a].hOffset) + inV[i].gl_VertexID * (binded_vao->heads[a].hStride),
						3*sizeof(float),
						&inV[i].attributes[a].v3);
				}
				if (binded_vao->heads[a].aType == AttributeType::VEC4)
				{
					getBufferData(binded_vao->heads[a].hBuffer,
						(binded_vao->heads[a].hOffset) + inV[i].gl_VertexID * (binded_vao->heads[a].hStride),
						4*sizeof(float),
						&inV[i].attributes[a].v4);
				}
			}
			
		}

		binded_prg->progVS( outV[i], inV[i], binded_prg->progUni);

		if ((i + 1) % 3 == 0)                         //Dalsi faze primo za behu cyklu, start pouze kazdy treti prubeh cyklu (trojuhelnik ma 3 vrcholy)
		{
			primiTriangle Triangle;
			Triangle.tV1 = outV[i - 2];      //Primitive assembly - pouze slozi 3 vrcholy do struktury
			Triangle.tV2 = outV[i - 1];
			Triangle.tV3 = outV[i];
			
			pTriangles.push_back(Triangle);
		}
		
	}

	primiTriangle selected_triangle;
	//for (auto& selected_triangle : pTriangles)
	while(!pTriangles.empty())
	{
		selected_triangle = pTriangles.back();
		pTriangles.pop_back();
		
		//printf("1. z:%f w:%f\n", selected_triangle.tV1.gl_Position.z, selected_triangle.tV1.gl_Position.w);
		//printf("2. z:%f w:%f\n", selected_triangle.tV2.gl_Position.z, selected_triangle.tV2.gl_Position.w);
		//printf("3. z:%f w:%f\n", selected_triangle.tV3.gl_Position.z, selected_triangle.tV3.gl_Position.w);


		//Clip space near plane orez

		uint8_t insideCount = 0;

		if (-(selected_triangle.tV1.gl_Position.w) > selected_triangle.tV1.gl_Position.z) insideCount+=1;
		if (-(selected_triangle.tV2.gl_Position.w) > selected_triangle.tV2.gl_Position.z) insideCount+=2;
		if (-(selected_triangle.tV3.gl_Position.w) > selected_triangle.tV3.gl_Position.z) insideCount+=4;

		if (insideCount) // pokud je nejaky vrchol mimo, tato podminka je pro optimalizaci (most common scenario, at nekontroluje dalsich x podminek)
		{
			if (insideCount == 1)
			{
				std::swap(selected_triangle.tV1, selected_triangle.tV3);
				insideCount = 4;
			}
			else if (insideCount == 2)
			{
				std::swap(selected_triangle.tV2, selected_triangle.tV3);
				insideCount = 4;
			}
			if (insideCount == 4) // only vertex 3 is inside
			{
				primiTriangle newTrg;
				newTrg.tV1 = selected_triangle.tV1;
				newTrg.tV2 = selected_triangle.tV3;

				float tA = (-selected_triangle.tV3.gl_Position.w - selected_triangle.tV3.gl_Position.z) / (selected_triangle.tV2.gl_Position.w - selected_triangle.tV3.gl_Position.w + selected_triangle.tV2.gl_Position.z - selected_triangle.tV3.gl_Position.z);
				float tB = (-selected_triangle.tV3.gl_Position.w - selected_triangle.tV3.gl_Position.z) / (selected_triangle.tV1.gl_Position.w - selected_triangle.tV3.gl_Position.w + selected_triangle.tV1.gl_Position.z - selected_triangle.tV3.gl_Position.z);

				for (int i = 0; i < 4; i++)
				{
					selected_triangle.tV3.gl_Position[i] = selected_triangle.tV3.gl_Position[i] + tA * (selected_triangle.tV2.gl_Position[i] - selected_triangle.tV3.gl_Position[i]);
					newTrg.tV2.gl_Position[i] = newTrg.tV2.gl_Position[i] + tB * (newTrg.tV1.gl_Position[i] - newTrg.tV2.gl_Position[i]);
				}

				newTrg.tV3 = selected_triangle.tV3;
				
				pTriangles.push_back(newTrg);
			}
			else if (insideCount == 3)
			{
				std::swap(selected_triangle.tV1, selected_triangle.tV3);
				insideCount = 6;
			}
			else if (insideCount == 5)
			{
				std::swap(selected_triangle.tV1, selected_triangle.tV2);
				insideCount = 6;
			}
			if (insideCount == 6) //only vertex 1 is outside
			{
				float t1 = (-selected_triangle.tV2.gl_Position.w - selected_triangle.tV2.gl_Position.z) / (selected_triangle.tV1.gl_Position.w - selected_triangle.tV2.gl_Position.w + selected_triangle.tV1.gl_Position.z - selected_triangle.tV2.gl_Position.z);
				float t2 = (-selected_triangle.tV3.gl_Position.w - selected_triangle.tV3.gl_Position.z) / (selected_triangle.tV1.gl_Position.w - selected_triangle.tV3.gl_Position.w + selected_triangle.tV1.gl_Position.z - selected_triangle.tV3.gl_Position.z);

				for (int i = 0; i < 4; i++)
				{
					selected_triangle.tV2.gl_Position[i] = selected_triangle.tV2.gl_Position[i] + t1 * (selected_triangle.tV1.gl_Position[i] - selected_triangle.tV2.gl_Position[i]);
					selected_triangle.tV3.gl_Position[i] = selected_triangle.tV3.gl_Position[i] + t1 * (selected_triangle.tV1.gl_Position[i] - selected_triangle.tV3.gl_Position[i]);
				}
			}
			else if (insideCount == 7) //all vertices are outside
			{
				continue;
			}


		}


		//Perspective division z CLIP SPACE do NDC (lze udelat matici, ale musela by se neustale menit kvuli zmene "w")
		selected_triangle.tV1.gl_Position.x = selected_triangle.tV1.gl_Position.x / selected_triangle.tV1.gl_Position.w;
		selected_triangle.tV1.gl_Position.y = selected_triangle.tV1.gl_Position.y / selected_triangle.tV1.gl_Position.w;
		selected_triangle.tV1.gl_Position.z = selected_triangle.tV1.gl_Position.z / selected_triangle.tV1.gl_Position.w;
		selected_triangle.tV2.gl_Position.x = selected_triangle.tV2.gl_Position.x / selected_triangle.tV2.gl_Position.w;
		selected_triangle.tV2.gl_Position.y = selected_triangle.tV2.gl_Position.y / selected_triangle.tV2.gl_Position.w;
		selected_triangle.tV2.gl_Position.z = selected_triangle.tV2.gl_Position.z / selected_triangle.tV2.gl_Position.w;
		selected_triangle.tV3.gl_Position.x = selected_triangle.tV3.gl_Position.x / selected_triangle.tV3.gl_Position.w;
		selected_triangle.tV3.gl_Position.y = selected_triangle.tV3.gl_Position.y / selected_triangle.tV3.gl_Position.w;
		selected_triangle.tV3.gl_Position.z = selected_triangle.tV3.gl_Position.z / selected_triangle.tV3.gl_Position.w;

		//z NDC do SCREEN SPACE (opet lze udelat matici ale zbytecna komplikace kvuli homogenni slozce)
		selected_triangle.tV1.gl_Position.x = (selected_triangle.tV1.gl_Position.x + 1.0f) * 0.5 * (screenWidth);
		selected_triangle.tV1.gl_Position.y = (selected_triangle.tV1.gl_Position.y + 1.0f) * 0.5 * (screenHeight);
		selected_triangle.tV2.gl_Position.x = (selected_triangle.tV2.gl_Position.x + 1.0f) * 0.5 * (screenWidth);
		selected_triangle.tV2.gl_Position.y = (selected_triangle.tV2.gl_Position.y + 1.0f) * 0.5 * (screenHeight);
		selected_triangle.tV3.gl_Position.x = (selected_triangle.tV3.gl_Position.x + 1.0f) * 0.5 * (screenWidth); // souradnice x i y nemuzou byt na 500x500 obrazovce v rozmezi 0-500,
		selected_triangle.tV3.gl_Position.y = (selected_triangle.tV3.gl_Position.y + 1.0f) * 0.5 * (screenHeight); //  to by bylo 501 sloupcu a radku, proto odcitam od width a height


		RasterizeTriangle(selected_triangle, screenWidth, screenHeight);
	}




	pTriangles.clear();
	delete [] inV;
	delete [] outV;
}

/// @}
