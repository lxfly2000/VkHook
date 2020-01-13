#include "custom_swapbuffers.h"
#include<map>
#include<ctime>
#include<string>

#define F(_i_str) (float)_wtof(_i_str)

class SwapBuffersDraw
{
private:
	unsigned t1, t2, fcounter;
	RECT windowrect;
	//FTDraw ftdraw;
	std::wstring display_text;
	int current_fps;
	TCHAR time_text[32], fps_text[32],width_text[32],height_text[32];

	char font_name[256];
	UINT font_size;
	TCHAR text_x[16], text_y[16], text_align[16], text_valign[16], display_text_fmt[256], fps_fmt[32], time_fmt[32],width_fmt[32],height_fmt[32];
	TCHAR font_red[16], font_green[16], font_blue[16], font_alpha[16];
	TCHAR font_shadow_red[16], font_shadow_green[16], font_shadow_blue[16], font_shadow_alpha[16], font_shadow_distance[16];
	int period_frames,font_face_index;

	//glm::vec4 text_color, text_shadow_color;
	float calc_text_x, calc_text_y, calc_shadow_x, calc_shadow_y;
	float anchor_x, anchor_y;
public:
	SwapBuffersDraw():t1(0),t2(0),fcounter(0),windowrect()
	{
	}
	void CalcRect(int x,int y,int width,int height)
	{
		windowrect.left = x;
		windowrect.top = y;
		windowrect.right = x+width;
		windowrect.bottom = y+height;
		calc_text_x = F(text_x)*width;
		calc_text_y = F(text_y)*height;
		calc_shadow_x = calc_text_x + F(font_shadow_distance);
		calc_shadow_y = calc_text_y + F(font_shadow_distance);
		//ftdraw.ResizeWindow(width, height);
	}
	bool Init(const VkSwapchainKHR* pSC,VkQueue* pCQ)
	{
		TCHAR szConfPath[MAX_PATH];
		char szConfPathA[MAX_PATH];
		GetDLLPath(szConfPath, ARRAYSIZE(szConfPath));
		GetDLLPathA(szConfPathA, ARRAYSIZE(szConfPathA));
		lstrcpy(wcsrchr(szConfPath, '.'), TEXT(".ini"));
		strcpy_s(strrchr(szConfPathA, '.'), 5, ".ini");
#define GetInitConfStr(key,def) GetPrivateProfileString(TEXT("Init"),TEXT(_STRINGIZE(key)),def,key,ARRAYSIZE(key),szConfPath)
#define GetInitConfStrA(key,def) GetPrivateProfileStringA("Init",_STRINGIZE(key),def,key,ARRAYSIZE(key),szConfPathA)
#define GetInitConfInt(key,def) key=GetPrivateProfileInt(TEXT("Init"),TEXT(_STRINGIZE(key)),def,szConfPath)
		size_t rlen;
		getenv_s(&rlen, font_name, "windir");
		strcat_s(font_name, "/Fonts/SimSun.ttc");
		GetInitConfStrA(font_name, font_name);
		GetInitConfInt(font_size, 48);
		GetInitConfStr(font_red, TEXT("1"));
		GetInitConfStr(font_green, TEXT("1"));
		GetInitConfStr(font_blue, TEXT("0"));
		GetInitConfStr(font_alpha, TEXT("1"));
		GetInitConfStr(font_shadow_red, TEXT("0.5"));
		GetInitConfStr(font_shadow_green, TEXT("0.5"));
		GetInitConfStr(font_shadow_blue, TEXT("0"));
		GetInitConfStr(font_shadow_alpha, TEXT("1"));
		GetInitConfStr(font_shadow_distance, TEXT("2"));
		GetInitConfInt(font_face_index, 0);
		GetInitConfStr(text_x, TEXT("0"));
		GetInitConfStr(text_y, TEXT("0"));
		GetInitConfStr(text_align, TEXT("left"));
		GetInitConfStr(text_valign, TEXT("top"));
		GetInitConfInt(period_frames, 60);
		GetInitConfStr(time_fmt, TEXT("%H:%M:%S"));
		GetInitConfStr(fps_fmt, TEXT("FPS:%3d"));
		GetInitConfStr(width_fmt, TEXT("%d"));
		GetInitConfStr(height_fmt, TEXT("%d"));
		GetInitConfStr(display_text_fmt, TEXT("{fps}"));
		//text_color = glm::vec4(F(font_red), F(font_green), F(font_blue), F(font_alpha));
		//text_shadow_color = glm::vec4(F(font_shadow_red), F(font_shadow_green), F(font_shadow_blue), F(font_shadow_alpha));

		GetClientRect(WindowFromDC(NULL), &windowrect);//TODO
		calc_text_x = F(text_x)*(windowrect.right-windowrect.left);
		calc_text_y = F(text_y)*(windowrect.bottom-windowrect.top);
		if (lstrcmpi(text_align, TEXT("left")) == 0)
			anchor_x = 0.0f;
		else if (lstrcmpi(text_align, TEXT("center")) == 0)
			anchor_x = 0.5f;
		else if (lstrcmpi(text_align, TEXT("right")) == 0)
			anchor_x = 1.0f;
		else
			anchor_x = F(text_align);
		if (lstrcmpi(text_valign, TEXT("top")) == 0)
			anchor_y = 0.0f;
		else if (lstrcmpi(text_valign, TEXT("center")) == 0)
			anchor_y = 0.5f;
		else if (lstrcmpi(text_valign, TEXT("bottom")) == 0)
			anchor_y = 1.0f;
		else
			anchor_y = F(text_valign);
		calc_shadow_x = calc_text_x + F(font_shadow_distance);
		calc_shadow_y = calc_text_y + F(font_shadow_distance);
		//ftdraw.Init(windowrect.right - windowrect.left, windowrect.bottom - windowrect.top, font_name, font_face_index, font_size, NULL);
		return true;
	}

	void Draw()
	{
		if (fcounter-- == 0)
		{
			fcounter = period_frames;
			t1 = t2;
			t2 = GetTickCount();
			if (t1 == t2)
				t1--;
			current_fps = period_frames * 1000 / (t2 - t1);
			wsprintf(fps_text, fps_fmt, current_fps);//注意wsprintf不支持浮点数格式化
			time_t t1 = time(NULL);
			tm tm1;
			localtime_s(&tm1, &t1);
			wcsftime(time_text, ARRAYSIZE(time_text), time_fmt, &tm1);
			wsprintf(width_text, width_fmt, windowrect.right - windowrect.left);
			wsprintf(height_text, height_fmt, windowrect.bottom - windowrect.top);
			display_text = display_text_fmt;
			size_t pos = display_text.find(TEXT("\\n"));
			if (pos != std::wstring::npos)
				display_text.replace(pos, 2, TEXT("\n"));
			pos = display_text.find(TEXT("{fps}"));
			if (pos != std::wstring::npos)
				display_text.replace(pos, 5, fps_text);
			pos = display_text.find(TEXT("{time}"));
			if (pos != std::wstring::npos)
				display_text.replace(pos, 6, time_text);
			pos = display_text.find(TEXT("{width}"));
			if (pos != std::wstring::npos)
				display_text.replace(pos, 7, width_text);
			pos = display_text.find(TEXT("{height}"));
			if (pos != std::wstring::npos)
				display_text.replace(pos, 8, height_text);
		}
		/*//https://github.com/ocornut/imgui/blob/master/examples/imgui_impl_opengl3.cpp#L142
#pragma region Backup GL state
		GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
		glActiveTexture(GL_TEXTURE0);
		GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
		GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
		GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
#ifdef GL_POLYGON_MODE
		GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
		GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
		GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
		GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
		GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
		GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
		GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
		GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
		GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
		GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
		GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
		GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
		GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
		bool clip_origin_lower_left = true;
#ifdef GL_CLIP_ORIGIN
		GLenum last_clip_origin; glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
		clip_origin_lower_left = (last_clip_origin == GL_LOWER_LEFT);
#endif
#pragma endregion
		//Notice that the origin point is at bottom-left of the screen
		ftdraw.RenderText(display_text.c_str(), calc_shadow_x, calc_shadow_y,anchor_x,anchor_y, 1.0f, text_shadow_color);
		ftdraw.RenderText(display_text.c_str(), calc_text_x, calc_text_y,anchor_x,anchor_y, 1.0f, text_color);
#pragma region Restore modified GL state
		glUseProgram(last_program);
		glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
		glBindSampler(0, last_sampler);
#endif
		glActiveTexture(last_active_texture);
		glBindVertexArray(last_vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
		glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
		if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
		glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
		OriginalViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
#pragma endregion*/
	}
	void ExecuteExtraCommandList()
	{
		//Currently nothing to do.
	}
};

struct SCV
{
	const VkSwapchainKHR* pSC;
	VkQueue* pCQ;
	SwapBuffersDraw* pPresent;
	bool NewPresent()
	{
		if (pPresent)
			delete pPresent;
		pPresent = new SwapBuffersDraw();
		return pPresent->Init(pSC, pCQ);
	}
	SCV() :pSC(nullptr), pCQ(nullptr), pPresent(nullptr)
	{
	}
	SCV(const VkSwapchainKHR* sc, VkQueue* cq) :SCV()
	{
		pSC = sc;
		pCQ = cq;
	}
	SCV(SCV&& other)noexcept :SCV()
	{
		pSC = other.pSC;
		pCQ = other.pCQ;
		pPresent = other.pPresent;
		other.pPresent = nullptr;
	}
	//这里会卡住，不知道为啥
	/*~SCV()
	{
		if (pPresent)
		{
			delete pPresent;
			pPresent = nullptr;
		}
	}*/
};
static std::map<const VkSwapchainKHR*, void*> s_d;
static std::map<VkQueue*, void*> c_d;
static std::map<void*, SCV> d_sc;

SCV& GetSCVorNew(void* pDev, const VkSwapchainKHR* pSC, VkQueue* pCQ)
{
	if (d_sc.find(pDev) == d_sc.end())
		d_sc.insert(std::make_pair(pDev, SCV(pSC, pCQ)));
	SCV& scv = d_sc[pDev];
	if (pSC)
		scv.pSC = pSC;
	if (pCQ)
		scv.pCQ = pCQ;
	return scv;
}


void CustomQueuePresentKHR(VkQueue q, const VkPresentInfoKHR* pInfo)
{
	/*//查找或记录设备
	void* pDev=nullptr;//TODO
	if (s_d.find(pInfo->pSwapchains) == s_d.end())
		s_d.insert(std::make_pair(pInfo->pSwapchains, pDev));
	//查找或记录SCV
	SCV& scv = GetSCVorNew(pDev, pInfo->pSwapchains, nullptr);
	if (scv.pCQ)
	{
		if (scv.pPresent)
			scv.pPresent->Draw();
		else if (s_d[pInfo->pSwapchains] == c_d[scv.pCQ])
			scv.NewPresent();
	}*/
}

void CustomSetViewport(VkCommandBuffer cb, uint32_t firstViewport, uint32_t vpCount, const VkViewport* pViewports)
{
	/*VkSwapchainKHR* p=nullptr;
	if (s_d.find(p) == s_d.end())
		return;
	void* d = s_d[p];
	if (d_sc.find(d) == d_sc.end())
		return;
	SwapBuffersDraw* pp = d_sc[d].pPresent;
	if (pp)
		pp->CalcRect(pViewports->x,pViewports->y,pViewports->width,pViewports->height);*/
}

void CustomExecuteCommands(VkCommandBuffer cb, uint32_t cbCount, const VkCommandBuffer* pCommandBuffers)
{
	/*//查找或记录设备
	void* pDev=nullptr;
	VkQueue* q;
	if (c_d.find(q) == c_d.end())
		c_d.insert(std::make_pair(q, pDev));
	//查找或记录SCV
	SCV& scv = GetSCVorNew(pDev, nullptr, q);
	if (scv.pSC)
	{
		if (scv.pPresent)
			scv.pPresent->ExecuteExtraCommandList();
		else if (s_d[scv.pSC] == c_d[q])
			scv.NewPresent();
	}*/
}

void CustomQueueSubmit(VkQueue q, uint32_t submitCount, const VkSubmitInfo* pSubmits, VkFence fence)
{
	//TODO
}