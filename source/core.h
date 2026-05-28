/* date = May 26th 2026 11:22 am */


#define ArrayCount(x) ((int)sizeof(x)/(int)sizeof(x[0]))



typedef uint32_t u32;
typedef uint8_t u8;


#define Kilobyte(x) x * 1024
#define Megabyte(x) Kilobyte(x) * 1024
#define Gigabyte(x) Megabyte(x) * 1024


internal_f u32 
c_str_size(const char* a)
{
	u32 size = 0;
	while(a[size++]);		
	return size - 1;
}

internal_f bool
c_str_compare(const char* _a, const char* _b)
{
	u8* a=(u8*)_a;
	u8* b=(u8*)_b;
	
	u32 a_size = c_str_size(_a);
	u32 b_size = c_str_size(_b);
	
	if(a_size != b_size)
	{
		return false;
	}
	
	
	for(u32 idx = 0; idx < a_size; ++idx)
	{
		if(a[idx] != b[idx])
		{
			return false;
		}
	}
	
	
	return true;
}

global_f void
BytesCopy(void* _s, void* _d, u32 size)
{
	u8* s = (u8*)_s;
	u8* d = (u8*)_d;
	
	for(u32 idx = 0; idx < size; ++idx)
	{
		d[idx] = s[idx];
	}
}

struct arena_t
{
	u8 *data;
	u32 current;
	u32 used;
	u32 size;	
};


internal_f void*
PushSize(arena_t *arena, u32 size)
{
	assert(arena->current + size < arena->size);
	
	u8* memory = arena->data + arena->current;
	arena->current += size;
	arena->used += size;
	
	return memory;
}

struct scratch_t
{	
	arena_t* parent;
	arena_t arena;
	u32 arena_prev;
	
	
	~scratch_t()
	{
		parent->current = arena_prev;		
	}
	
};

global_f void
InitArena(arena_t *arena, u32 size, void* memory)
{
	arena->data = (u8*)memory;
	arena->current = 0;
	arena->used = 0;
	arena->size = size;
}


internal_f arena_t* 
ScratchBegin(arena_t *arena, scratch_t *scratch)
{			
	scratch->arena_prev = arena->current;	
	scratch->arena.size = arena->size - arena->used;
	scratch->arena.used = 0;
	
	assert(scratch->arena.size > 0);
	
	scratch->parent = arena;
	
	InitArena(&scratch->arena, arena->size - arena->used, arena->data + arena->current);
	
	return &scratch->arena;
}



#define SCRATCH(arena) \
scratch_t scratch; \
arena_t* temp_arena = ScratchBegin(arena, &scratch); \

