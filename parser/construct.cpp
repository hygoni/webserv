#include "Context.hpp"

void begin(Context& ctx) {
	ctx.level++;
	ctx.save.push(ctx.idx);
}

void end(Context& ctx) {
	ctx.level--;
}

void group(Context& ctx) {
	if (ctx.state) {
		ctx.level++;
		ctx.save.push(ctx.idx);
	}
}

void ungroup(Context& ctx) {
	if (ctx.skip == ctx.level) {
		ctx.skip = -1;
	}
	if (ctx.state == false) {
		ctx.idx = ctx.save.top(); ctx.save.pop();
	}
	ctx.level--;
}

void repeat(Context& ctx, int min, int max, void (*f)(Context&)) {
	if (ctx.skip <= ctx.level) {
		return ;
	}
	if (ctx.state) {
		int i;
		/* max == 0이면 실패할 때까지 */
		for (i = 0; max == 0 ? true : i < max; i++) {
			ctx.save.push(ctx.idx);
			f(ctx);
			if (ctx.state == false) {
				ctx.idx = ctx.save.top(); ctx.save.pop();
				break;
			}
		}
		ctx.state = ((i >= min) && (max == 0 ? true :  i <= max));
	}
}

void option(Context& ctx, void (*f)(Context&)) {
	group(ctx);
	repeat(ctx, 0, 1, f);
	ungroup(ctx);
}

void accept(Context& ctx, bool (*f)(Context&)) {
	if (ctx.skip <= ctx.level) {
		return ;
	}
	ctx.state = f(ctx);
}

void alternate(Context& ctx) {
	if (ctx.skip <= ctx.level) {
		return ;
	}
	if (ctx.state) {
		ctx.skip = ctx.level;
	} else {
		ctx.idx = ctx.save.top(); ctx.save.pop();
		ctx.state = true;
	}
}
