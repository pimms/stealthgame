#include "ShadowLayer.h"
#include "LightSource.h"
#include "ShadowCaster.h"
#include "Shader.h"
#include "GameLayer.h"


/*
================
ShadowLayer Public
===============
*/
ShadowLayer::ShadowLayer(GameLayer *gameLayer)
	: 	_shader(NULL),
		_renderTexture(NULL),
		_gameLayer(gameLayer)
{
	_renderTexture = new RenderTexture(Vec2(1280.f, 720.f));

	_shader = new Shader("res/shd/light.frag", "res/shd/light.vert");
	_shader->SetUniform1i("tex", 0);
	_shader->SetUniform1f("lalpha", 0.8f);

	SetShadowColor(Color(0.f, 0.f, 0.f, 0.9f));
}

ShadowLayer::~ShadowLayer()
{
	if (_renderTexture)
		delete _renderTexture;
	if (_shader)
		delete _shader;
}


void ShadowLayer::AddLightSource(LightSource *obj)
{
	_lightSources.push_back(obj);
}

void ShadowLayer::RemoveLightSource(LightSource *obj)
{
	for (int i=0; i<_shadowCasters.size(); i++) {
		if (_lightSources[i] == obj) {
			_lightSources.erase(_lightSources.begin()+i--);
		}
	}
}


void ShadowLayer::AddShadowCaster(ShadowCaster *obj)
{
	_shadowCasters.push_back(obj);
}

void ShadowLayer::RemoveShadowCaster(ShadowCaster *obj)
{
	for (int i=0; i<_shadowCasters.size(); i++) {
		if (_shadowCasters[i] == obj) {
			_shadowCasters.erase(_shadowCasters.begin()+i--);
		}
	}
}


void ShadowLayer::Render(Renderer *renderer)
{
	_renderTexture->BindFBO();

	glEnable(GL_STENCIL_TEST);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderer->PushTransform();
	renderer->ApplyTransform(_gameLayer);

	for (int i=0; i<_lightSources.size(); i++) {
		if (ShouldDrawLight(_lightSources[i])) {
			RenderShadows(_lightSources[i], renderer);
			RenderLight(_lightSources[i], renderer);
		}
	}

	glDisable(GL_STENCIL_TEST);
	renderer->PopTransform();

	DrawFillShadow();
	_renderTexture->UnbindFBO();

	glPushMatrix();
	glLoadIdentity();
	DrawRenderTexture();
	glPopMatrix();
}


void ShadowLayer::SetShadowColor(Color c)
{
	_shadowColor = c;
	_shader->SetUniform4f("ulcolor", c.r, c.g, c.b, c.a);
}


/* 
================
ShadowLayer Private
================
*/
void ShadowLayer::RenderLight(LightSource *source, Renderer *renderer)
{
	glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	source->RenderLight(renderer);
}

void ShadowLayer::RenderShadows(LightSource *source, Renderer *renderer)
{	
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_NEVER, 0x1, 0x0);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

	vector<Vec2> shadowverts = GetShadowVertices(source);

	glColor4f(_shadowColor.r, _shadowColor.g, _shadowColor.b, 1.f);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &shadowverts[0]);
	glDrawArrays(GL_QUADS, 0, shadowverts.size());
	glDisableClientState(GL_VERTEX_ARRAY);

	glColor4ub(255, 255, 255, 255);
}


vector<Vec2> ShadowLayer::GetShadowVertices(LightSource *source)
{
	vector<Vec2> shadow;

	for (int i=0; i<_shadowCasters.size(); i++) {
		ShadowCaster *caster = _shadowCasters[i];
		if (!WillLightHitCaster(source, caster))
			continue;

		vector<Vec2> shape = caster->GetShadowShape();
		Vec2 casterPos = caster->GetGameObject()->WorldPosition();
		Vec2 p = source->GetGameObject()->WorldPosition();
		
		// Push P1, P2, P2', P1' to create a quad
		for (int j=0; j<shape.size(); j++) {
			vector<Vec2> quad;
			quad.resize(4);
			for (int k=0; k<2; k++) {
				int idx = (k + j) % shape.size();
				Vec2 d;
				d.x += shape[idx].x - p.x;
				d.y += shape[idx].y - p.y;

				d.x *= 1000.f;
				d.y *= 1000.f;
				
				// Push the shape vertex and the delta
				quad[k] = shape[idx];
				quad[3-k] = d;
			}

			for (int k=0; k<4; k++)
				shadow.push_back(quad[k]);
		}
	}

	return shadow;
}

void ShadowLayer::DrawFillShadow()
{
	glColor4f(_shadowColor.r, _shadowColor.g, _shadowColor.b, _shadowColor.a);
	Vec2 r = _renderTexture->GetResolution();
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);

	float v[8] = {
		0.f,	0.f,	
		r.x,	0.f,
		r.x,	r.y,
		0.f,	r.y,	
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, v);
	glDrawArrays(GL_QUADS, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);

	glColor4ub(255,255,255,255);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ShadowLayer::DrawRenderTexture()
{
	Vec2 r = _renderTexture->GetResolution();

	float v[8] = {
		0.f, 0.f,
		r.x, 0.f,
		r.x, r.y,
		0.f, r.y,
	};

	float t[8] = {
		0.f, 0.f,
		1.f, 0.f,
		1.f, 1.f,
		0.f, 1.f,
	};

	glEnable(GL_TEXTURE_2D);
	_renderTexture->BindTex();

	_shader->Enable();

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glColor4ub(255, 255, 255, 255);

	glVertexPointer(2, GL_FLOAT, 0, v);
	glTexCoordPointer(2, GL_FLOAT, 0, t);
	glDrawArrays(GL_QUADS, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	_shader->Disable();
}


bool ShadowLayer::ShouldDrawLight(LightSource *source)
{
	Vec2 lp = source->Position();
	float rad = source->GetLightRadius();
	Rect l(	lp.x - rad, lp.y - rad, rad, rad);

	lp = _gameLayer->Position();
	Rect s(-lp.x, -lp.y, 1280.f, 720.f);

	return l.Overlaps(s);
}

bool ShadowLayer::WillLightHitCaster(LightSource *source, ShadowCaster *caster)
{
	Vec2 lp = source->Position();
	float rad = source->GetLightRadius();
	Rect l(	lp.x - rad, lp.y - rad, rad*2, rad*2);

	Rect c = caster->GetAABB();

	return l.Overlaps(c);
}
