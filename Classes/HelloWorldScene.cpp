#include "HelloWorldScene.h"

USING_NS_CC;

static Texture2D * texture2d;

//--------------------------------------------------------
static const GLchar* s_szColorSpriteVSH = 
	"													\n\
	attribute vec4 a_position;							\n\
	attribute vec2 a_texCoord;							\n\
														\n\
	#ifdef GL_ES										\n\
	varying mediump vec2 v_texCoord;					\n\
	#else												\n\
	varying vec2 v_texCoord;							\n\
	#endif												\n\
														\n\
	void main()											\n\
	{													\n\
		gl_Position = CC_MVPMatrix * a_position;		\n\
		v_texCoord = a_texCoord;						\n\
	}";													
//--------------------------------------------------------
static const GLchar* s_szColorSpriteFSH = 
	"																													\n\
	#ifdef GL_ES 								 																		\n\
		precision mediump float; 					 																	\n\
	#endif 																												\n\
																														\n\
	uniform sampler2D u_texture; 				 																		\n\
	varying vec2 v_texCoord; 					 																		\n\
																														\n\
	void main(void) 							 																		\n\
	{ 											 																		\n\
		// vec3( 0.299, 0.587, 0.114 ) 是RGB转YUV的参数值，生成灰色图														\n\
		float MixColor = dot(texture2D(u_texture, v_texCoord).rgb, vec3(0.299, 0.587, 0.114));							\n\
		// 使用灰色图进行颜色混合																							\n\
		vec4 blendColor = vec4( 1.0, 1.0, 1.0, 1.0 ); // 调整这个值以修改最终混合色值										\n\
		gl_FragColor = vec4(MixColor * blendColor.r, MixColor * blendColor.g, MixColor * blendColor.b, blendColor.a);	\n\
	}";

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", 24);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);

	 texture2d = TextureCache::sharedTextureCache()->addImage("HelloWorld.png");
    
	 texture2d->getGLProgram()->initWithByteArrays(s_szColorSpriteVSH, s_szColorSpriteFSH);

	

		texture2d->getGLProgram()->link();  
		CHECK_GL_ERROR_DEBUG();  

		texture2d->getGLProgram()->updateUniforms();  
		CHECK_GL_ERROR_DEBUG();  


    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	m_customCommand.init(_globalZOrder);  
    m_customCommand.func = CC_CALLBACK_0(HelloWorld::onDraw, this, transform, flags);  
    renderer->addCommand(&m_customCommand);
}

void HelloWorld::onDraw(const Mat4 &transform, uint32_t flags)
{
	//ccGLBlendFunc( m_sBlendFunc.src, m_sBlendFunc.dst );  
	static GLfloat vertext[] = {
		0.0f,0.0f,0.0f,
		480.0f,0.0f,0.0f,
		0.0f,320.0f,0.0f,
		480.0f,320.0f,0.0f
	};

	//但是纹理的读取也是逆时针，只是EG是用三角形作为基础图形去绘制其他图形，方向先绘制下三角，再绘制上三角！！！之前这里搞错了。希望大家别被我误导了
	static GLfloat coord[] = {
		0.0f,1.0f,
		1.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f
	};

	static GLfloat colors[] = {
		1.0f,1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,1.0f,
	};

	//初始化纹理，即将纹理映射至openGL ES的世界坐标系中
	
	coord[2] = coord[6] = texture2d->getMaxS();
	coord[1] = coord[3] = texture2d->getMaxT();

//开启着色器,坐标、颜色、纹理
	GL::enableVertexAttribs( GL::VERTEX_ATTRIB_FLAG_POS_COLOR_TEX);
        //设置着色器
	
	//目前没看懂具体执行了的use()操作,启用可编程管线？？
	texture2d->getGLProgram()->use();
	//设置统一属性，即M模型、V视图、P投影，完成从Opengl的世界坐标到屏幕坐标的映射
	texture2d->getGLProgram()->setUniformsForBuiltins();

	

	//绑定纹理,texture2d->getName() 纹理在内存或显存中存储的下标（应该是下标，我没看纹理加载,还不了解GPU和CPU的通信）
	glBindTexture(GL_TEXTURE_2D,texture2d->getName());

	//设置顶点,每个顶点向量维数为3，数据类型为GL_FLOAT，不归一化（真实物理坐标不能归一化），两个个顶点间的偏移量为0,

	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, 0, vertext);
	
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, coord);
	
	glVertexAttribPointer(GLProgram::VERTEX_ATTRIB_COLOR,4,GL_FLOAT,GL_FALSE,0,colors);


	//绘图，从下标0开始，有4个顶点
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	
}