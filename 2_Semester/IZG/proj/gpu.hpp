/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * @author Michal Pyšík, xpysik00@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <vector>
#include <memory>


/**
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();


	struct primiTriangle {   //tato struktura musi byt uz zde, protoze ji vyuziva RasterizeTriangle funkce
		OutVertex tV1;
		OutVertex tV2;
		OutVertex tV3;
	};

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);
	void	  putPixel				 (glm::vec3 pPosition, glm::vec4 pColor, uint32_t swidth, uint32_t sheight);
	bool	  isPixelCloser			 (glm::vec3 pPosition, uint32_t swidth, uint32_t sheight); //funkce otestuje zda ma novy fragment mensi hloubku nez pixel co je jiz ve FrameBufferu
	float     getMax				 (float a, float b);  //nejak mi nefungovaly funkce na max a min tak jsem udelal vlastni
	float     getMin				 (float a, float b);
	void	  RasterizeTriangle	     (primiTriangle &triangle, const uint32_t width, const uint32_t height);  //rasterizace pomoci baryc. souradnic
	float     InterpolateAttribute	 (float lamb0, float lamb1, float lamb2, float h0, float h1, float h2, float A0, float A1, float A2);
	float     edgeFunction			 (const glm::vec4 &a, const glm::vec4 &b, const glm::vec4 &c);


    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
    /// @}

	struct headStr {
		BufferID hBuffer;
		uint64_t hOffset;
		uint64_t hStride;
		AttributeType aType;
		bool enabled;
	}; //struktura hlavy, kazda vao jich ma presne 16


	struct vTable {
		bool isIndexing;
		IndexType iType;
		BufferID indexBuffer;
		headStr heads[maxAttributes];
	}; //struktura vao = 16 hlav + indexovaci buffer + typ indexovani

	struct progSettings {
		VertexShader progVS;
		FragmentShader progFS;
		Uniforms progUni;			//struktura shader programu = ptr na VertexShader + ptr na FragmentShader + tabulka uniform +...
		AttributeType progVFAT[maxAttributes];    //...+ typ atributu na interpolaci (VFAT = Vertex Fragment Attribute Type)
	};

	struct ColorPixel {		//struktura barevneho pixelu, pixely jsou ale ulozeny v poli po bytech, ne primo jako struct
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t alpha;
	};

	struct FrameBuffer {          //struktura framebufferu
		int fbHeight;
		int fbWidth;
		uint8_t *cPixels;
		float *dPixels;
	};


	vTable *binded_vao; //aktivni (binded) VertexPuller
	progSettings *binded_prg; //aktivni shader program
	FrameBuffer *binded_fb; //momentalni stav framebufferu

	std::vector<std::unique_ptr<uint8_t[]>> buffers; //pole bufferu s identifikatorama
	std::vector<std::unique_ptr<vTable>> pullerTables; //to stejne pro vao tabulky
	std::vector<std::unique_ptr<progSettings>> prgArray; //a pro shader programy

};


