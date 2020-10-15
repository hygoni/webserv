/* Copyright 2020 hyeyoo, hyekim */

#include <climits>
#include "construct.hpp"
#include "Context.hpp"

void begin(Context& ctx) {
    ctx.level++;
    ctx.save.push(ctx.idx);
}

void end(Context& ctx) {
    ctx.level--;
	if ((unsigned long)(ctx.idx) != ctx.str.size())
		ctx.state = false;
	ctx.save.pop();
}

void group(Context& ctx) {
    if (!ctx.state)
		return;
	ctx.level++;
	ctx.save.push(ctx.idx);
}

void ungroup(Context& ctx) {
	if (!ctx.state)
		return ;
    if (ctx.skip == ctx.level) {
        ctx.skip = INT_MAX;
    }
	ctx.save.pop();
    ctx.level--;
}

void repeat(Context& ctx, int min, int max, bool (*f)(Context&)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    }
	int i;
	/* max == 0이면 실패할 때까지 */
	for (i = 0; max == 0 ? true : i < max; i++) {
		ctx.save.push(ctx.idx);
		accept(ctx, f);
		if (ctx.state == false) {
			ctx.idx = ctx.save.top(); ctx.save.pop();
			break;
		}
		ctx.save.pop();
	}
	ctx.state = ((i >= min) && (max == 0 ? true :  i <= max));
}

void repeat(Context& ctx, char ch, int min, int max, bool (*f)(Context&, char)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    }
	int i;
	/* max == 0이면 실패할 때까지 */
	for (i = 0; max == 0 ? true : i < max; i++) {
		ctx.save.push(ctx.idx);
		accept(ctx, ch, f);
		if (ctx.state == false) {
			ctx.idx = ctx.save.top(); ctx.save.pop();
			break;
		}
		ctx.save.pop();
	}
	ctx.state = ((i >= min) && (max == 0 ? true :  i <= max));
}

void repeat(Context& ctx, bool b, int min, int max, bool (*f)(Context&, bool)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    }
	int i;
	/* max == 0이면 실패할 때까지 */
	for (i = 0; max == 0 ? true : i < max; i++) {
		ctx.save.push(ctx.idx);
		accept(ctx, b, f);
		if (ctx.state == false) {
			ctx.idx = ctx.save.top(); ctx.save.pop();
			break;
		}
		ctx.save.pop();
	}
	ctx.state = ((i >= min) && (max == 0 ? true :  i <= max));
}

void repeat(Context& ctx, std::string str, int min, int max, bool (*f)(Context&, std::string)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    }
	int i;
	/* max == 0이면 실패할 때까지 */
	for (i = 0; max == 0 ? true : i < max; i++) {
		ctx.save.push(ctx.idx);
		accept(ctx, str, f);
		if (ctx.state == false) {
			ctx.idx = ctx.save.top(); ctx.save.pop();
			break;
		}
		ctx.save.pop();
	}
	ctx.state = ((i >= min) && (max == 0 ? true :  i <= max));
}

void option(Context& ctx, bool (*f)(Context&)) {
    group(ctx);
    repeat(ctx, 0, 1, f);
    ungroup(ctx);
}

void option(Context& ctx, char ch, bool (*f)(Context&, char)) {
    group(ctx);
    repeat(ctx, ch, 0, 1, f);
    ungroup(ctx);
}

void option(Context& ctx, bool b, bool (*f)(Context&, bool)) {
    group(ctx);
    repeat(ctx, b, 0, 1, f);
    ungroup(ctx);
}

void option(Context& ctx, std::string str, bool (*f)(Context&, std::string)) {
    group(ctx);
    repeat(ctx, str, 0, 1, f);
    ungroup(ctx);
}

void accept(Context& ctx, bool (*f)(Context&)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    } else if (ctx.str.size() == 0) {
		ctx.state = false;
		return;
	}
    ctx.state = f(ctx);
}

void accept(Context& ctx, char ch, bool (*f)(Context&, char)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    } else if (ctx.str.size() == 0) {
		ctx.state = false;
		return;
	}
    ctx.state = f(ctx, ch);
}

void accept(Context& ctx, bool b, bool (*f)(Context&, bool)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    } else if (ctx.str.size() == 0) {
		ctx.state = false;
		return;
	}
    ctx.state = f(ctx, b);
}

void accept(Context& ctx, std::string str, bool (*f)(Context&, std::string)) {
    if (ctx.skip <= ctx.level || !ctx.state) {
        return;
    } else if (ctx.str.size() == 0) {
		ctx.state = false;
		return;
	}
    ctx.state = f(ctx, str);
}

void alternate(Context& ctx) {
    if (ctx.skip <= ctx.level) {
        return;
    }
    if (ctx.state) {
        ctx.skip = ctx.level;
    } else {
        ctx.idx = ctx.save.top();
        ctx.state = true;
    }
}
