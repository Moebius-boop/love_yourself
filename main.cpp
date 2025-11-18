#include <SDL2/SDL.h>
#include <stdio.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include<string.h>

const int SCREEN_W = 2560;
const int SCREEN_H = 1600;
Uint8 *audio_buf;
Uint32 audio_len;
Uint32 audio_pos = 0;
SDL_AudioDeviceID device_id;

// 生成更好的随机数
uint32_t better_rand() {
    return (rand() << 16) | rand();  // 组合两个rand()
}

void callback(void* userdate, Uint8* stream, int len)
{
	if (!audio_buf || audio_len == 0) {
		memset(stream, 0, len);  // 填充静音
		return;
	}
	int remain = audio_len - audio_pos;
	if (remain <= 0) {
		// 音频已播放完毕，填充静音
		memset(stream, 0, len);
		audio_pos = 0;  // 重置位置以便重新播放
		return;
	}
	if (remain >= len)
	{
		SDL_memcpy(stream, audio_buf + audio_pos, len);
		audio_pos += len;
	}
	else
	{
		SDL_memcpy(stream, audio_buf + audio_pos, remain);
		memset(stream + remain, 0, len - remain);
		audio_pos = 0;
	}
}

char say[7][50] = {
	"有没有想我？",
	"我不在的时候有没有好好吃饭？",
	"天气冷了，多加衣服呀",
	"要对自己好一点啊！",
	"想我了的话记得看看天空",
	"我一直都在...",
	"关闭我吧......"
};

void Event(SDL_Window *window1,TTF_Font *font1,const char *i, SDL_Renderer* renderer,SDL_Texture **text_texture,SDL_Rect *text_rect,SDL_Surface* text_surf)
{
	//SDL_HideWindow(window1);
	//SDL_Delay(500);

    // 1. 先淡出（逐渐变黑）
    for(int alpha = 255; alpha >= 0; alpha -= 5) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // 绘制半透明的原内容
        SDL_SetTextureAlphaMod(*text_texture, alpha);  // 如果支持透明度
        SDL_RenderCopy(renderer, *text_texture, NULL, text_rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

    SDL_Delay(100);
	//更改窗口位置
	SDL_SetWindowPosition(window1, rand() % 1200, 100+rand() % 700);

	//覆盖原来内容
    SDL_SetRenderDrawColor(renderer,0,0,0,255);

	// 双缓冲绘制开始
	// 1. 清除后台缓冲区
	SDL_RenderClear(renderer);

    text_surf = TTF_RenderUTF8_Blended(font1, i, { 255,255,255 });

    *text_rect = {(425 - text_surf->w) / 2,(175 - text_surf->h) / 2,text_surf->w,text_surf->h };

    *text_texture = SDL_CreateTextureFromSurface(renderer, text_surf);
    //更新屏幕内容
    SDL_RenderCopy(renderer,*text_texture,NULL,text_rect);
    
    for(int alpha = 0; alpha <= 255; alpha += 5) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        SDL_SetTextureAlphaMod(*text_texture, alpha);
        SDL_RenderCopy(renderer, *text_texture, NULL, text_rect);
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }

	//SDL_Delay(500);
	//SDL_ShowWindow(window1);
    SDL_RenderPresent(renderer);

}

char TEXT[60][30] = { "按时吃饭呀","Take care always","天凉多穿衣","累了就歇歇",
"记得多喝水","Did you sleep well?","别熬夜啦","出门带雨伞","路上慢一点",
"I miss you so","压力别太大","开心最重要","多吃点蔬果","不舒服说哦",
"Don't push hard","记得想我呀","注意防晒呀","忙完报平安","Have a nice day",
"一切都会好","别吃太冰的","好好照顾自己","睡前泡泡脚","工作别太累",
"You are safe now","记得添衣物","饿了就加餐","雨天别出门","保持好心情",
"I’m here for you","别太焦虑啦","多喝温热水","想你啦宝贝","记得吃早餐","早点休息呀",
"Take it easy dear","出门戴口罩","累了靠靠我","别忘吃 meds","今天开心吗",
"You matter to me","天热防中暑","别省着吃饭","遇事别硬扛","记得常联系",
"Rest well tonight","降温添衣服","别熬夜追剧","记得润嗓子","有我陪着你",
"You’re not alone","少吃辛辣的","多喝热汤呀","记得做核酸","别太累自己",
"Sleep tight dear","雨天路小心","按时吃药呀","保持好心态","Wish you well always"};


#undef main

int main() 
{
    srand(time(NULL));

    //初始化视频与音频
    SDL_Init(SDL_INIT_VIDEO| SDL_INIT_AUDIO);
    SDL_Window* window[200];

    //初始化ttf
    TTF_Init();

    //导入wav
    SDL_AudioSpec audio_spec;
    if (NULL == SDL_LoadWAV("D:\\music\\M500003Z9S3z3lAzNk.wav", &audio_spec, &audio_buf, &audio_len))
    {  
        SDL_Log("SDL_LoadWAV failed :%s", SDL_GetError());
        return -1;
    }

    //定义播放回调函数
    audio_spec.callback = callback;

    //打开音频设备
    device_id = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);

    //开始播放
    SDL_PauseAudioDevice(device_id, 0);

    SDL_Delay(800);

    //创建第一个窗口
    SDL_Window * window1;
    window1 = SDL_CreateWindow("LOVE", (1600 - 425) / 2, (800 - 175) / 2, 425, 175, 0);

    // 创建渲染器（启用双缓冲和垂直同步）
    SDL_Renderer* renderer = SDL_CreateRenderer(window1, -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //获取surface
    SDL_Surface *surf1 = SDL_GetWindowSurface(window1);

    //标定一个区域用于填充背景颜色
    SDL_Rect rect1 = { 0,0,425,175 };
    SDL_FillRect(surf1, &rect1, 0x000000);

    //文字获取
    TTF_Font *font1 = TTF_OpenFont("C:\\Windows\\Fonts\\STHUPO.TTF", 30);

    //将文字与surface联系起来
    SDL_Surface* text1_surf = TTF_RenderUTF8_Blended(font1, "在忙嘛？", { 255,255,255 });
    SDL_Rect text_rect = { (425 - text1_surf->w) / 2,(175 - text1_surf->h) / 2,text1_surf->w,text1_surf->h };

    //填充文字
    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer,text1_surf); 

    //更新屏幕内容
    SDL_RenderCopy(renderer,text_texture,NULL,&text_rect);

    SDL_RenderPresent(renderer);

    int index = 0;

    bool running = true;

    while (running)
    {
        SDL_Event event1;
        while(SDL_PollEvent(&event1))
        {
            if (event1.type == SDL_QUIT)
            {
                if (index >= 7)
                {
                    running = false;
                }
                else
                {
                    Event(window1, font1, say[index], renderer,&text_texture,&text_rect,text1_surf);
                    index++;
                }
            }
        }
    }
    SDL_DestroyWindow(window1);

    SDL_Delay(700);

    for (int i = 0;i < 200;i++)
    {
        //定义窗口的随机初始位置
        int x = rand() % (SCREEN_W - 1200);
        int y = rand() % (SCREEN_H - 600);
        
        //创建窗口
        window[i] = SDL_CreateWindow("Love Yourself", x, y, 350, 100, 0);

        //获取window的surface
        SDL_Surface* surf = SDL_GetWindowSurface(window[i]);

        //规定一个区域为全window，填充随机颜色
        SDL_Rect rect = { 0, 0, 350, 100 };
        SDL_FillRect(surf, &rect, SDL_MapRGB(surf->format, rand() % 256, rand() % 256, rand() % 256));

        //打开字体
        TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\STHUPO.TTF", 24);
        //渲染字体
        
        //获取与窗口关联的surface
        SDL_Surface* text_surf = TTF_RenderUTF8_Blended(font,TEXT[rand() % 60], { (Uint8)(rand() % 256),(Uint8)(rand() % 256), (Uint8)(rand() % 256)});
        //将字体surface复制到窗口surface上
        SDL_Rect text_rect = { (350 - text_surf->w) / 2,(100 - text_surf->h) / 2,text_surf->w,text_surf->h };
        SDL_BlitSurface(text_surf, NULL, surf, &text_rect);
        

        //更新windows屏幕
        SDL_UpdateWindowSurface(window[i]);
        SDL_Delay(50);

        TTF_CloseFont(font);
        //释放surface
        SDL_FreeSurface(text_surf);
        
    }

    for (int i = 0;i < 200;i++)
    {
        SDL_DestroyWindow(window[i]);
    }
    


    // 创建窗口
    int width = 600;
    int heigh = width *2052.0/1440.0;

    SDL_Window* window2 = SDL_CreateWindow("Love Yourself",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         600, heigh,
                                         SDL_WINDOW_HIDDEN);
    if(!window) {
        printf("窗口创建失败: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    printf("success\n");

    SDL_Rect rect = {0,0,width,heigh};
    
    
    //图片
    SDL_Surface *bmp_surf = SDL_LoadBMP("玫瑰花1.bmp");
    if(NULL == bmp_surf)
    {
        SDL_Log("bmp_surf failed:%s",SDL_GetError());
        return 0;
    }

    SDL_Surface *surf = SDL_GetWindowSurface(window2);
    if(NULL == surf)
    {
        SDL_Log("surf failed:%s",SDL_GetError());
        return 0;
    }

    SDL_BlitScaled(bmp_surf,NULL,surf,&rect);

    size_t size = heigh * surf->pitch/4;

    Uint32 *px=(Uint32*)surf->pixels;

    Uint32 *origincolor = (Uint32*)calloc(size,sizeof(Uint32));
    Uint32 *flag_arr = (Uint32*)calloc(size,sizeof(Uint32));

    for(int i =0; i<size;i++)
    {
        origincolor[i]=px[i];
    }

    SDL_ShowWindow(window2);

    //变成全灰色
    SDL_FillRect(surf,&rect,SDL_MapRGB(surf->format,64,64,64));
    SDL_UpdateWindowSurface(window2);
    SDL_Event event;
    bool flag = 0;
    size_t index1 = 0;
    while(!flag)
    {
        if(index1%100==0)
        {
            SDL_UpdateWindowSurface(window2);
        }
        if(SDL_PollEvent(&event)&&event.type==SDL_QUIT)
        {
            break;
        }
        // 在循环中使用
        size_t m = better_rand() % size;
        if(!flag_arr[m]) {
            flag_arr[m] = 1;
            index1++;
            px[m] = origincolor[m];
        }
        if(index1==size)
        {
            flag = true;
        }
    }
    SDL_UpdateWindowSurface(window2);
    printf("%d",index1);
    // 清理资源
    //SDL_DestroyTexture(texture);
    
    SDL_FreeSurface(bmp_surf);
    SDL_FreeSurface(surf);
    //SDL_FreeSurface(txt_surf);
   // TTF_CloseFont(font);
    SDL_DestroyWindow(window2);
    SDL_Quit();
    
    return 0;
}