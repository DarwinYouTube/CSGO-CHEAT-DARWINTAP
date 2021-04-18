#pragma once

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color);

void DrawLine(int x1, int y1, int x2, int y2, int thickness, bool antialias, D3DCOLOR color);

void DrawBox(int x, int y, int w, int h, int thickness, bool antialias, D3DCOLOR color);

void CornerBox(int x, int y, int w, int h, int thickness, float cornered, bool antialias, D3DCOLOR color);

void DrawCircle(float x, float y, float radius, float sides, int thickness, bool antialias, D3DCOLOR color);