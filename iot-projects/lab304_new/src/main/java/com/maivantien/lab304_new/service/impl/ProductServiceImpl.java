package com.maivantien.lab304_new.service.impl;
import lombok.AllArgsConstructor;

import org.springframework.stereotype.Service;

import com.maivantien.lab304_new.dto.ProductDto;
import com.maivantien.lab304_new.entity.Category;
import com.maivantien.lab304_new.entity.Product;
import com.maivantien.lab304_new.service.ProductService;
import com.maivantien.lab304_new.repository.CategoryRepository;
import com.maivantien.lab304_new.repository.ProductRepository;
import java.util.List;
import java.util.Optional;
@Service
@AllArgsConstructor
public class ProductServiceImpl implements ProductService {
    private ProductRepository productRepository;
    private CategoryRepository  categoryRepository;
    @Override
    public Product createProduct(ProductDto product) {
        Category category = categoryRepository.findById(product.getCategory_id()).orElse(null);
        if(category == null) {
            return null;
        }
        Product newProduct = new Product();
        newProduct.setTitle(product.getTitle());
        newProduct.setDescription(product.getDescription());
        newProduct.setPhoto(product.getPhoto());
        newProduct.setPrice(product.getPrice());
        newProduct.setCategory(category);
        return productRepository.save(newProduct);
    }
    @Override
    public Product getProductById(Long productId) {
    Optional<Product> optionalProduct = productRepository.findById(productId);
        return optionalProduct.get();
    }
    @Override
    public List<Product> getAllProducts() {
        return productRepository.findAll();
    }
    @Override
    public Product updateProduct(Product product) {
        Product existingProduct = productRepository.findById(product.getId()).get();
        existingProduct.setTitle(product.getTitle());
        existingProduct.setDescription(product.getDescription());
        existingProduct.setPhoto(product.getPhoto());
        existingProduct.setPrice(product.getPrice());
        existingProduct.setCategory(product.getCategory());
        Product updatedProduct = productRepository.save(existingProduct);
        return updatedProduct;
    }
    @Override
    public void deleteProduct(Long productId) {
        productRepository.deleteById(productId);
    }
}