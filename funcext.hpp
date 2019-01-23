#pragma once
#include<iterator>
namespace stle {
	template<typename Wrapper>
	class MapMixture {
	public:
		template<typename F>
		auto map(F&&f)const ->decltype(make_map(std::move(*(Wrapper*)(this)), std::forward<F>(f))){
			return make_map(std::move(*(Wrapper*)(this)), std::forward<F>(f));
		}
	};
	template<typename Wrapper>
	class FilterMixture {
	public:
		template<typename F>
		auto filter(F&&f) const ->decltype(make_filter(std::move(*(Wrapper*)(this)), std::forward<F>(f))) {
			return make_filter(std::move(*(Wrapper*)(this)),std::forward<F>(f));
		}
	};
	template<typename Wrapper>
	class ReduceMixture {
	public:
		template<typename Value,typename F>
		auto reduce(Value&&v,F&&f)const->Value{
			//return make_reduce<Wrapper,Ret,F>(std::move(*(Wrapper*)(this)),std::forward<F>(f));
			return make_reduce(std::forward<Value>(v),std::move(*(Wrapper*)(this)), std::forward<F>(f));
		}
	};
	template<typename Wrapper>
	class CollectMixture {
	public:
		template<typename OutContainer>
		auto collect()const ->OutContainer {
			return make_collect<OutContainer>(std::move(*(Wrapper*)(this)));
		}
	};
	template<typename Iter>
	class StreamWrapper:
		public std::iterator<std::forward_iterator_tag, typename std::iterator_traits<Iter>::value_type>,
		public MapMixture<StreamWrapper<Iter>>,
		public FilterMixture<StreamWrapper<Iter>>,
		public ReduceMixture<StreamWrapper<Iter>>,
		public CollectMixture <StreamWrapper<Iter>>{
	protected:
		mutable Iter bp;
		Iter ep;
	public: 
		StreamWrapper(Iter bp, Iter ep) :bp(bp), ep(ep) {}
		StreamWrapper(StreamWrapper&&wrapper):bp(std::move(wrapper.bp)),ep(wrapper.ep){}
		//using value_type = typename std::iterator_traits<Iter>::value_type;
		operator bool()const {
			return bp != ep;
		}
		StreamWrapper& operator++() {
			++bp;
			return *this;
		}
		auto operator*()const ->decltype(*bp){
			return *bp;
		}
	
	};
	template<typename LastWrapper,typename F>
	class MapWrapper :
		public std::iterator<std::forward_iterator_tag,typename std::result_of<std::remove_all_extents_t<F>>>,
		public MapMixture<MapWrapper<LastWrapper,F>>,
		public FilterMixture<MapWrapper<LastWrapper,F>>,
		public ReduceMixture<MapWrapper<LastWrapper,F>>,
		public CollectMixture<MapWrapper<LastWrapper,F>>{
	protected:
		mutable LastWrapper wrapper;
		std::remove_all_extents_t<F> f{ nullptr };
	public:
		MapWrapper(LastWrapper&&wrapper, F&&f) :wrapper(std::forward<LastWrapper>(wrapper)), f(std::forward<F>(f)) {}
		MapWrapper(MapWrapper&&wrapper) :wrapper(std::forward<LastWrapper>(wrapper.wrapper)), f(std::forward<F>(wrapper.f)) {}
		MapWrapper(const MapWrapper& wrapper) : wrapper(wrapper.wrapper), f(wrapper.f) {}
		operator bool()const {
			return wrapper;
		}
		MapWrapper& operator++() {
			++wrapper;
			return *this;
		}
		auto operator*()const->decltype(f(*wrapper)){
			return f(*wrapper);
		}
	
	};
	template<typename LastWrapper,typename F>
	class FilterWrapper :
		public std::iterator<std::forward_iterator_tag,typename std::iterator_traits<LastWrapper>::value_type>,
		public MapMixture<FilterWrapper<LastWrapper,F>>,
		public FilterMixture<FilterWrapper<LastWrapper,F>>,
		public ReduceMixture<FilterWrapper<LastWrapper,F>> ,
		public CollectMixture<FilterWrapper<LastWrapper,F>>{
	public:
		
		FilterWrapper(LastWrapper&&wrapper, const F&f) :wrapper(std::move(wrapper)), f(f) {}
		FilterWrapper(const LastWrapper&wrapper, const F&f) :wrapper(wrapper), f(wrapper.f) {}
		FilterWrapper(FilterWrapper&&wrapper) :wrapper(std::move(wrapper.wrapper)), f(wrapper.f) {}
		FilterWrapper(const FilterWrapper& wrapper) : wrapper(wrapper.wrapper), f(wrapper.f) {}
		operator bool()const {
			return wrapper;
		}
		FilterWrapper& operator++() {
			static_assert(std::is_same<decltype(f(*wrapper)),bool>::value, "return value must is bool.");
			++wrapper;
			while (wrapper && !std::invoke(f, *wrapper)) ++wrapper;
			return *this;
		}
		auto operator*()const ->value_type{
			while (wrapper && !std::invoke(f, *wrapper)) ++wrapper;
			return *wrapper;
		}
	protected:
		mutable LastWrapper wrapper;
		std::remove_all_extents_t<F> f;
	};
	template<typename LastWrapper,typename F>
	auto make_map(LastWrapper&&wrapper,F&&f){
		return MapWrapper<LastWrapper, F>(std::forward<LastWrapper>(wrapper), std::forward<F>(f));
	}
	template<typename LastWrapper, typename F>
	auto make_filter(LastWrapper&&wrapper, F&&f) {
		return FilterWrapper<LastWrapper, F>(std::forward<LastWrapper>(wrapper), std::forward<F>(f));
	}
	template<typename LastWrapper,typename Value, typename F>
	auto make_reduce(Value&& v,LastWrapper&&wrapper, F&&f) {
		//using RetVal = Ret;
		//RetVal ret{};
		//using Value = decltype(*wrapper);
		Value ret = std::forward<Value>(v);
		while (wrapper)
		{
			ret = std::invoke(f,ret, *wrapper);
			++wrapper;
		}
		return std::move(ret);
	}
	template<typename OutContainer,typename LastWrapper>
	auto make_collect(LastWrapper&&wrapper)->OutContainer {
		OutContainer container;
		auto outer = std::back_inserter(container);
		while (wrapper)
		{
			*(++outer) = *(wrapper);
			++wrapper;
		}
		return std::move(container);
	}
	template<typename Container>
	auto to_stream(Container&&container) ->StreamWrapper<decltype(std::begin(container))> {
		using Iter = decltype(std::begin(container));
		return StreamWrapper<Iter>(std::begin(container), std::end(container));
	}
}
