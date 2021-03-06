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
		template <typename F>
		void for_each(F&&f)const{
			auto &wrapper = *(Wrapper *)(this);
			while (wrapper)
			{
				//*(++outer) = *(wrapper);
				f(*wrapper);
				++wrapper;
			}
		}
	};
	template<typename Wrapper>
	class SplitMixture{
		public:
		 
		  template<class Container,class F>
		  auto split(F&&f)const {
			  return make_split<Container>(std::move(*(Wrapper *)(this)), std::forward<F>(f));
		  }
	};
	template<typename Implatment>
	class BaseWrapper ://public std::iterator<std::forward_iterator_tag, typename std::result_of_t<F> >,
		public MapMixture<Implatment>,
		public FilterMixture<Implatment>,
		public ReduceMixture<Implatment>,
		public CollectMixture<Implatment>,
		public SplitMixture<Implatment> {

	};
	template <typename Iter>
	class StreamWrapper :
		public BaseWrapper<StreamWrapper<Iter>>,
	 	public std::iterator<std::forward_iterator_tag, typename std::iterator_traits<Iter>::value_type>
	{
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
	template <typename LastWrapper, typename Ret,typename F>
	class MapWrapper : public BaseWrapper<MapWrapper<LastWrapper,Ret, F>>,
					   public std::iterator<std::forward_iterator_tag,Ret>
	/* public std::iterator<std::forward_iterator_tag, typename std::result_of<std::remove_all_extents_t<F>>>,
					   public MapMixture<MapWrapper<LastWrapper, F>>,
					   public FilterMixture<MapWrapper<LastWrapper, F>>,
					   public ReduceMixture<MapWrapper<LastWrapper, F>>,
					   public CollectMixture<MapWrapper<LastWrapper, F>>,
					   public SplitMixture<MapWrapper<LastWrapper, F>>*/
	{
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
	template <typename LastWrapper, typename F>
	class FilterWrapper :
		public BaseWrapper<FilterWrapper<LastWrapper, F>>,
	 	public std::iterator<std::forward_iterator_tag, typename std::iterator_traits<LastWrapper>::value_type>
	{
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
	template <typename OutContainer, typename LastWrapper, typename F>
	class SplitWrapper : public BaseWrapper<SplitWrapper<OutContainer, LastWrapper, F>>,
	 public std::iterator<std::forward_iterator_tag, typename OutContainer>
	{
	  public:
		//using value_type = OutContainer;
		SplitWrapper(LastWrapper &&wrapper, const F &f) : wrapper(std::move(wrapper)), f(f) {}
		SplitWrapper(SplitWrapper&&spwrapper):wrapper(std::move(spwrapper.wrapper)),f(std::move(f)){}
		operator bool() const
		{
			
			while (wrapper && std::invoke(f, *wrapper)) ++wrapper;
			return wrapper;
		}
		SplitWrapper& operator++(){
			//if(!wrapper){
			//	return *this;
			//}
			//++wrapper;
			auto v = std::move(container);
			while (wrapper && std::invoke(f, *wrapper)) ++wrapper;
			auto outIter = std::back_inserter(container);
			while (wrapper && !std::invoke(f, *wrapper)){
				*(++outIter) = std::move(*wrapper);
				++wrapper;
			}
			return *this;
		}
		auto operator*() const->OutContainer&{
			auto outIter = std::back_inserter(container);
			//while (wrapper && std::invoke(f, *wrapper)) ++wrapper;
			while (isBegin&&wrapper && !std::invoke(f, *wrapper))
			{
					*(++outIter) = std::move(*wrapper);
					++wrapper;
			}
			isBegin = false;
			return container;
		}
		protected :
			mutable LastWrapper wrapper;
			std::remove_all_extents_t<F> f;
			mutable OutContainer container;
			mutable bool    isBegin{ true };
	};
		template <typename LastWrapper, typename F>
		auto make_map(LastWrapper &&wrapper, F &&f)
		{
			using Ret = decltype(f(*wrapper));
			return MapWrapper<LastWrapper,Ret, F>(std::forward<LastWrapper>(wrapper), std::forward<F>(f));
		}
		template <typename LastWrapper, typename F>
		auto make_filter(LastWrapper &&wrapper, F &&f)
		{
			return FilterWrapper<LastWrapper, F>(std::forward<LastWrapper>(wrapper), std::forward<F>(f));
		}
		template <typename LastWrapper, typename Value, typename F>
		auto make_reduce(Value &&v, LastWrapper &&wrapper, F &&f)
		{
			//using RetVal = Ret;
			//RetVal ret{};
			//using Value = decltype(*wrapper);
			Value ret = std::forward<Value>(v);
			while (wrapper)
			{
				ret = std::invoke(f, ret, std::move(*wrapper));
				++wrapper;
			}
			return std::move(ret);
		}
		template <typename OutContainer, typename LastWrapper>
		auto make_collect(LastWrapper &&wrapper) -> OutContainer
		{
			OutContainer container;
			auto outer = std::back_inserter(container);
			while (wrapper)
			{
				*(++outer) = std::move(*(wrapper));
				++wrapper;
			}
			return std::move(container);
		}
		template <typename Container>
		auto to_stream(Container &&container) -> StreamWrapper<decltype(std::begin(container))>
		{
			using Iter = decltype(std::begin(container));
			return StreamWrapper<Iter>(std::begin(container), std::end(container));
		}
		template<typename Iterator>
		auto to_range(Iterator bit,Iterator eit)->StreamWrapper<Iterator>{
			return StreamWrapper<Iterator>(bit,eit);
		}

		template <typename OutContainer, typename LastWrapper, typename F>
		auto make_split(LastWrapper &&wrapper, F &&f) -> SplitWrapper<OutContainer, LastWrapper, F>
		{
			return SplitWrapper<OutContainer, LastWrapper, F>(std::forward<LastWrapper>(wrapper), std::forward<F>(f));
		}
}
